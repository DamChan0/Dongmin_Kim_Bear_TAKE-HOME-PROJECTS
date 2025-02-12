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
    EXIT
};

User::User(FlowModule& fm) : flowModule(fm), running(true)
{
    userThread = std::thread(&User::processUserInput, this);
}

User::~User()
{
    running = false;
    if (userThread.joinable())
    {
        userThread.join();
    }
}

void User::onPinVerificationResult(bool pinVerified)
{
    if (pinVerified)
    {
        std::cout << "[User] PIN Verified!" << std::endl;
        this->pinVerified = true;
    }
    else
    {
        std::cout << "[User] PIN Incorrect!" << std::endl;
        this->pinVerified = false;
    }
}

void User::processUserInput()
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
            std::cout << "[ATM] System is now ON" << std::endl;
        }
        else
        {
            std::cout << "[ATM] Failed to start" << std::endl;
        }
    }
    else
    {
        std::cout << "ATM is off" << std::endl;
        return;
    }
    std::cout << "[User] ATM is ON\n";
    std::cout << "running: " << running << std::endl;
    while (running)
    {
        std::cout << "\n1. Insert Card\n2. Enter PIN\n3. Deposit\n4. Withdraw\n5. Check "
                     "Balance\n6. Exit\nSelect an option: ";

        std::cout << "\n--- ATM STATUS ---\n";
        std::cout << "Card Inserted: " << (cardInserted ? "Yes" : "No") << "\n";
        std::cout << "PIN Verified: " << (pinVerified ? "Yes" : "No") << "\n";
        std::cout << "Account Selected: " << (accountSelected ? "Yes" : "No") << "\n";
        std::cout << "------------------\n";

        int choice;
        std::cin >> choice;

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
                          NULL};
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
                          NULL};
                flowModule.addCommand(newCmd);
                break;

            case UserInput::CHECK_BALANCE:
                if (!pinVerified)
                {
                    std::cout << "[ERROR] Please verify your PIN first!" << std::endl;
                    break;
                }
                else
                {
                    std::promise<double> balancePromise;
                    std::future<double> balanceFuture = balancePromise.get_future();
                    newCmd = {CommandType::ATM_CHECK_BALANCE,
                              "User",
                              "FlowModule",
                              cardNumber,
                              0,
                              0,
                              &balancePromise};
                    flowModule.addCommand({newCmd});
                    double balance = balanceFuture.get();

                    std::cout << "[User] Current Balance: $" << balance << std::endl;
                }
                break;

            case UserInput::EXIT:
                std::cout << "Exiting ATM..." << std::endl;
                running = false;
                break;

            default:
                std::cout << "[ERROR] Invalid option. Try again." << std::endl;
        }
    }
}
