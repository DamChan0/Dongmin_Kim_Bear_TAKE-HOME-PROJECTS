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
        if (cmd.RX == "FlowModule")
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
                            cmd.type, "FlowModule", "ATM", cmd.cardNumber, cmd.pin, 0,
                            0,        cmd.callback};

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

                    case CommandType::ATM_CHECK_BALANCE:
                        std::cout << "[FlowModule] Checking balance for "
                                  << cmd.cardNumber << std::endl;
                        newCmd = {CommandType::ATM_CHECK_BALANCE,
                                  "FlowModule",
                                  "ATM",
                                  cmd.cardNumber,
                                  0,
                                  0,
                                  NULL,
                                  cmd.callback};
                        commandQueue.push(newCmd);
                        break;
                    default:
                        break;
                }
            }
        }
    }
}
