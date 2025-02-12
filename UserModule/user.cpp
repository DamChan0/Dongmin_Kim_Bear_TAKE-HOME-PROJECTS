#include "user.h"

#include <flowModule.h>

#include <iostream>

#include "BankAPI.h"

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

void User::processUserInput()
{
    std::cout << "[User] ATM is ON\n";
    std::cout << "running: " << running << std::endl;
    while (running)
    {
        std::cout << "\n1. Insert Card\n2. Enter PIN\n3. Deposit\n4. Withdraw\n5. Check "
                     "Balance\n6. Exit\nSelect an option: ";
        int choice;
        std::cin >> choice;

        switch (choice)
        {
            case 1:
                if (cardInserted)
                {
                    std::cout << "[User] Card is already inserted!" << std::endl;
                    break;
                }
                std::cout << "Enter card number: ";
                std::cin >> cardNumber;
                flowModule.addUserCommand(
                    {CommandType::USER_INSERT_CARD, cardNumber, 0, 0});
                cardInserted = true;
                break;

            case 2:
                if (!cardInserted)
                {
                    std::cout << "[User] Please insert a card first!" << std::endl;
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
                flowModule.addUserCommand(
                    {CommandType::USER_VERIFY_PIN, cardNumber, pin, 0});
                pinVerified = true;  // 실제 검증은 BankAPI에서 수행됨
                break;

            case 3:
                if (!pinVerified)
                {
                    std::cout << "[User] Please verify your PIN first!" << std::endl;
                    break;
                }
                double depositAmount;
                std::cout << "Enter deposit amount: ";
                std::cin >> depositAmount;
                flowModule.addUserCommand(
                    {CommandType::USER_DEPOSIT, cardNumber, 0, depositAmount});
                break;

            case 4:
                if (!pinVerified)
                {
                    std::cout << "[User] Please verify your PIN first!" << std::endl;
                    break;
                }
                double withdrawAmount;
                std::cout << "Enter withdrawal amount: ";
                std::cin >> withdrawAmount;
                flowModule.addUserCommand(
                    {CommandType::USER_WITHDRAW, cardNumber, 0, withdrawAmount});
                break;

            case 5:
                if (!pinVerified)
                {
                    std::cout << "[User] Please verify your PIN first!" << std::endl;
                    break;
                }
                else
                {
                    // double balance = BankAPI::getBalance(cardNumber);
                    // std::cout << "[User] Current Balance: $" << balance << std::endl;
                }
                break;

            case 6:
                running = false;
                break;

            default:
                std::cout << "[User] Invalid option. Try again." << std::endl;
        }
    }
}
