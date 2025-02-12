#include "flowModule.h"

#include <BankAPI.h>

#include <future>
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

void FlowModule::addCommand(const Command &cmd)
{
    commandQueue.push(cmd);
}

void FlowModule::processCommands()
{
    while (running)
    {
        // 1. Process User → ATM commands
        if (commandQueue.empty())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        Command cmd = commandQueue.pop();
        if (cmd.RX != "FlowModule")
        {
            commandQueue.push(cmd);
        }
        Command newCmd;
        if (cmd.TX == "User")
        {
            {
                switch (cmd.type)
                {
                    case CommandType::USER_INSERT_CARD:
                        std::cout << "[FlowModule] Inserting card: " << cmd.cardNumber
                                  << std::endl;
                        newCmd = {CommandType::USER_INSERT_CARD,
                                  "FlowModule",
                                  "ATM",
                                  cmd.cardNumber,
                                  0,
                                  0,
                                  NULL};
                        commandQueue.push(newCmd);
                        break;

                    case CommandType::USER_VERIFY_PIN:
                        std::cout << "[FlowModule] Verifying PIN for " << cmd.cardNumber
                                  << std::endl;
                        newCmd = {
                            cmd.type,    "FlowModule", "ATM", cmd.cardNumber, cmd.pin, 0,
                            0,
                            cmd.callback  // ✅ 올바르게 전달됨을 보장
                        };

                        if (cmd.callback == nullptr)
                        {
                            std::cerr << "[ERROR] Callback is nullptr in FlowModule!"
                                      << std::endl;
                        }
                        commandQueue.push(newCmd);
                        break;

                    case CommandType::USER_DEPOSIT:
                        std::cout << "[FlowModule] Depositing $" << cmd.amount << " to "
                                  << cmd.cardNumber << std::endl;

                        newCmd = {CommandType::USER_DEPOSIT,
                                  "FlowModule",
                                  "ATM",
                                  cmd.cardNumber,
                                  0,
                                  cmd.amount,
                                  NULL};
                        commandQueue.push(newCmd);
                        break;

                    case CommandType::USER_WITHDRAW:
                        std::cout << "[FlowModule] Withdrawing $" << cmd.amount
                                  << " from " << cmd.cardNumber << std::endl;
                        newCmd = {CommandType::USER_WITHDRAW,
                                  "FlowModule",
                                  "ATM",
                                  cmd.cardNumber,
                                  0,
                                  cmd.amount,
                                  NULL};
                        commandQueue.push(newCmd);
                        break;

                    default:
                        break;
                }
            }

            // 2. Process ATM → BankAPI commands
            // if (cmd.TX == "ATM")
            // {
            //     switch (cmd.type)
            //     {
            //         case CommandType::ATM_DEPOSIT:
            //             BankAPI::deposit(cmd.cardNumber, cmd.amount);
            //             double balance = BankAPI::getBalance(cmd.cardNumber);
            //             std::cout << "[FlowModule] New balance: $" << balance
            //                       << std::endl;
            //             break;
            //         case CommandType::ATM_WITHDRAW:
            //             BankAPI::withdraw(cmd.cardNumber, cmd.amount);
            //             double balance = BankAPI::getBalance(cmd.cardNumber);
            //             std::cout << "[FlowModule] New balance: $" << balance
            //                       << std::endl;
            //             break;
            //         case CommandType::ATM_CHECK_BALANCE:
            //             BankAPI::getBalance(cmd.cardNumber);
            //             break;
            //         case CommandType::ATM_LOAD_ACCOUNT_INFO:
            //             BankAPI::loadAccounts();
            //             break;
            //         default:
            //             break;
            //     }
            // }
        }
    }
}
