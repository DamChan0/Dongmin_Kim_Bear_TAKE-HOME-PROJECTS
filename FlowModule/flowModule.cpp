#include "flowModule.h"

#include <BankAPI.h>

#include <iostream>

FlowModule::FlowModule()
{
    running = true;
    flowThread = std::thread(&FlowModule::processCommands, this);
}

FlowModule::~FlowModule()
{
    running = false;
    flowThread.join();
}

void FlowModule::addUserCommand(const Command &cmd)
{
    userCommandQueue.push(cmd);
}

void FlowModule::addBankCommand(const Command &cmd)
{
    bankCommandQueue.push(cmd);
}

void FlowModule::processCommands()
{
    while (running)
    {
        // 1. Process User → ATM commands
        if (!userCommandQueue.empty())
        {
            Command cmd = userCommandQueue.pop();

            switch (cmd.type)
            {
                case CommandType::USER_INSERT_CARD:
                    std::cout << "[FlowModule] Inserting card: " << cmd.cardNumber
                              << std::endl;
                    break;

                case CommandType::USER_VERIFY_PIN:
                    std::cout << "[FlowModule] Verifying PIN for " << cmd.cardNumber
                              << std::endl;
                    if (BankAPI::verifyPin(cmd.cardNumber, cmd.pin))
                    {
                        std::cout << "[FlowModule] PIN Verified!" << std::endl;
                    }
                    else
                    {
                        std::cout << "[FlowModule] PIN Incorrect!" << std::endl;
                    }
                    break;

                case CommandType::USER_DEPOSIT:
                    std::cout << "[FlowModule] Depositing $" << cmd.amount << " to "
                              << cmd.cardNumber << std::endl;
                    addBankCommand(cmd);
                    break;

                case CommandType::USER_WITHDRAW:
                    std::cout << "[FlowModule] Withdrawing $" << cmd.amount << " from "
                              << cmd.cardNumber << std::endl;
                    addBankCommand(cmd);
                    break;
                default:
                    break;
            }
        }

        // 2. Process ATM → BankAPI commands
        if (!bankCommandQueue.empty())
        {
            Command cmd = bankCommandQueue.pop();
            switch (cmd.type)
            {
                case CommandType::ATM_DEPOSIT:
                    BankAPI::deposit(cmd.cardNumber, cmd.amount);
                    break;
                case CommandType::ATM_WITHDRAW:
                    BankAPI::withdraw(cmd.cardNumber, cmd.amount);
                    break;
                case CommandType::ATM_CHECK_BALANCE:
                    // Not implemented
                    break;
                default:
                    break;
            }
        }
    }
}
