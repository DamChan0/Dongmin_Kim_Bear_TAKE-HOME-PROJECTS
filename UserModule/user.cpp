#include "user.h"

#include <flowModule.h>

#include <future>
#include <iostream>
#include <memory>

#include "BankAPI.h"

enum class UserInput
{
    UNKNOWN,
    INSERT_CARD,
    ENTER_PIN,
    DEPOSIT,
    WITHDRAW,
    CHECK_BALANCE,
    SELECT_ACCOUNT,
    EXIT
};

User::User(FlowModule& fm) : flowModule(fm), running(true)
{
    userThread = std::thread(&User::userInput, this);
}

void User::onPinVerificationResult(bool& pinVerified)
{
    if (pinVerified)
    {
        std::cout << "[User] PIN Verified!" << std::endl;
        this->pinVerified = true;
        isWaiting = false;
    }
    else
    {
        std::cout << "[User] PIN Incorrect!" << std::endl;
        this->pinVerified = false;
        isWaiting = false;
    }
}
void User::onSelectedAccount(uint64_t accountNumber)
{
    if (accountNumber)
    {
        std::cout << "[User] Account Selected!" << std::endl;
        this->accountSelected = true;
        this->accountNumber = accountNumber;
        isWaiting = false;
    }
    else
    {
        std::cout << "[User] Account not found!" << std::endl;
        this->accountSelected = false;
        isWaiting = false;
    }
}
void User::onCheckBalance(double balance)
{
    std::cout << "[User] Current Balance: $" << balance << std::endl;
}

void User::userInput()
{
    std::cout << "Are you turn on ATM?" << std::endl;
    std::cout << "1. Yes\n2. No\nSelect an option: ";
    int choice;
    std::cin >> choice;
    if (choice == 1)
    {
        std::promise<bool> atmOnPromise = std::promise<bool>();
        std::future<bool> atmOnFuture = atmOnPromise.get_future();
        Command cmdd = {CommandType::ATM_ON, "User", "ATM", "", 0, 0, &atmOnPromise};
        flowModule.addCommand(cmdd);

        running = atmOnFuture.get();
        if (running)
        {
            std::cout << "[User] System is now ON" << std::endl;
        }
        else
        {
            std::cout << "[User] Failed to start" << std::endl;
        }
    }
    else
    {
        std::cout << "ATM is off" << std::endl;
        return;
    }

    while (running)
    {
        while (isWaiting)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        std::cout << "\n1. Insert Card\n2. Enter PIN\n3. Deposit\n4. Withdraw\n5. Check "
                     "Balance\n6. Select Account\n7. Exit\n";

        std::cout << "\n--- ATM STATUS ---\n";
        std::cout << "Card Inserted: " << (cardInserted ? "Yes" : "No") << "\n";
        std::cout << "PIN Verified: " << (this->pinVerified ? "Yes" : "No") << "\n";
        std::cout << "Account Selected: " << (accountSelected ? "Yes" : "No") << "\n";
        std::cout << "------------------\n";

        std::cout << "Select an option: " << std::endl;

        int choice;
        std::cin >> choice;
        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        Command newCmd = {CommandType::UNKNOWN, "User", "FlowModule", "", 0, 0, NULL};
        UserInput input = static_cast<UserInput>(choice);
        std::promise<bool> pinPromise;
        std::future<bool> pinFuture = pinPromise.get_future();
        switch (input)
        {
            case UserInput::INSERT_CARD:
                if (cardInserted)
                {
                    std::cout << "[User] Card is already inserted!" << std::endl;
                    break;
                }
                std::cout << "Enter card number: ";
                std::cin >> cardNumber;

                newCmd = {CommandType::USER_INSERT_CARD,
                          "User",
                          "FlowModule",
                          cardNumber,
                          0,
                          0,
                          NULL};
                flowModule.addCommand(newCmd);
                this->cardNumber = cardNumber;
                cardInserted = true;
                break;

            case UserInput::ENTER_PIN:
            {
                if (!cardInserted)
                {
                    std::cout << "[ERROR] Please insert a card first!" << std::endl;
                    break;
                }
                if (pinVerified)
                {
                    std::cout << "[User] PIN is already verified!" << std::endl;
                    break;
                }
                uint64_t pin;
                std::cout << "Enter PIN: ";
                std::cin >> pin;

                newCmd = {CommandType::USER_VERIFY_PIN,
                          "User",
                          "FlowModule",
                          cardNumber,
                          pin,
                          0,
                          0,
                          [this](bool pinVerified)
                          { onPinVerificationResult(pinVerified); }};

                flowModule.addCommand(newCmd);
                isWaiting = true;

                break;
            }

            case UserInput::DEPOSIT:
                if (!pinVerified)
                {
                    std::cout << "[ERROR] Please verify your PIN first!" << std::endl;
                    break;
                }
                double depositAmount;
                std::cout << "Enter deposit amount: ";
                std::cin >> depositAmount;
                newCmd = {CommandType::USER_DEPOSIT,
                          "User",
                          "FlowModule",
                          cardNumber,
                          0,
                          depositAmount,
                          NULL,
                          0,
                          this->accountNumber};
                flowModule.addCommand(newCmd);
                break;

            case UserInput::WITHDRAW:
                if (!pinVerified)
                {
                    std::cout << "[ERROR] Please verify your PIN first!" << std::endl;
                    break;
                }
                double withdrawAmount;
                std::cout << "Enter withdrawal amount: ";
                std::cin >> withdrawAmount;
                newCmd = {CommandType::USER_WITHDRAW,
                          "User",
                          "FlowModule",
                          cardNumber,
                          0,
                          withdrawAmount,
                          NULL,
                          0,
                          this->accountNumber};
                flowModule.addCommand(newCmd);
                break;

            case UserInput::CHECK_BALANCE:
                if (!pinVerified)
                {
                    std::cout << "[ERROR] Please verify your PIN first!" << std::endl;
                    break;
                }
                if (!accountSelected)
                {
                    std::cout << "[ERROR] Please select an account first!" << std::endl;
                    break;
                }
                else
                {
                    double balance = 0;
                    newCmd = {CommandType::ATM_CHECK_BALANCE,
                              "User",
                              "FlowModule",
                              cardNumber,
                              0,
                              0,
                              nullptr,
                              [this](bool balance) { onCheckBalance(balance); },
                              this->accountNumber};

                    flowModule.addCommand({newCmd});

                    std::cout << "[User] Current Balance: $" << balance << std::endl;
                }
                break;

            case UserInput::SELECT_ACCOUNT:

                if (!pinVerified)
                {
                    std::cout << "[ERROR] Please verify your PIN first!" << std::endl;
                    break;
                }
                if (accountSelected)
                {
                    std::cout << "[ERROR] Account is already selected!" << std::endl;
                    break;
                }
                else
                {
                    newCmd = {CommandType::ATM_LOAD_ACCOUNT_INFO,
                              "User",
                              "FlowModule",
                              cardNumber,
                              0,
                              0,
                              nullptr,
                              nullptr};
                    flowModule.addCommand(newCmd);
                }
                std::cout << "Enter account number: ";
                std::cin >> accountNumber;
                newCmd = {CommandType::USER_SELECT_ACCOUNT,
                          "User",
                          "FlowModule",
                          cardNumber,
                          0,
                          0,
                          nullptr,
                          [this](uint64_t accountNumber)
                          { onSelectedAccount(accountNumber); },
                          accountNumber};
                flowModule.addCommand(newCmd);
                isWaiting = true;
                break;

            case UserInput::EXIT:
                std::cout << "Exiting ATM..." << std::endl;
                running = false;
                newCmd = {CommandType::ATM_OFF,
                          "User",
                          "ATM",
                          cardNumber,
                          0,
                          0,
                          nullptr,
                          nullptr};
                flowModule.addCommand(newCmd);
                return;
            default:
                std::cout << "[ERROR] Invalid option. Try again." << std::endl;
                break;
        }
    }
}
