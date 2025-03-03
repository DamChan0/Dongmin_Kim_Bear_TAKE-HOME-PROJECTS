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
                        // std::cout << "[FlowModule] Inserting card: " << cmd.cardNumber

                        newCmd = {.type = CommandType::USER_INSERT_CARD,
                                  .TX = "FlowModule",
                                  .RX = "ATM",
                                  .cardNumber = cmd.cardNumber,
                                  .pin = 0,
                                  .amount = 0,
                                  .data = NULL,
                                  .callback = NULL,
                                  .accountNumber = 0

                        };

                        commandQueue.push(newCmd);
                        break;

                    case CommandType::USER_VERIFY_PIN:
                        // std::cout << "[FlowModule] Verifying PIN for " <<
                        // cmd.cardNumber
                        //   << std::endl;
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
                        // std::cout << "[FlowModule] Depositing $" << cmd.amount << " to
                        // "
                        //   << cmd.cardNumber << std::endl;

                        newCmd = {CommandType::USER_DEPOSIT,
                                  "FlowModule",
                                  "ATM",
                                  cmd.cardNumber,
                                  cmd.pin,
                                  cmd.amount,
                                  cmd.data,
                                  cmd.callback,
                                  cmd.accountNumber};

                        commandQueue.push(newCmd);
                        break;

                    case CommandType::USER_WITHDRAW:
                        // std::cout << "[FlowModule] Withdrawing $" << cmd.amount
                        //           << " from " << cmd.cardNumber << std::endl;
                        newCmd = {CommandType::USER_WITHDRAW,
                                  "FlowModule",
                                  "ATM",
                                  cmd.cardNumber,
                                  cmd.pin,
                                  cmd.amount,
                                  cmd.data,
                                  cmd.callback,
                                  cmd.accountNumber};
                        commandQueue.push(newCmd);
                        break;

                    case CommandType::ATM_CHECK_BALANCE:
                        // std::cout << "[FlowModule] Checking balance for "
                        //           << cmd.cardNumber << std::endl;
                        newCmd = {CommandType::ATM_CHECK_BALANCE,
                                  "FlowModule",
                                  "ATM",
                                  cmd.cardNumber,
                                  0,
                                  0,
                                  NULL,
                                  cmd.callback,
                                  cmd.accountNumber};
                        commandQueue.push(newCmd);
                        break;

                    case CommandType ::ATM_LOAD_ACCOUNT_INFO:
                        // std::cout << "[FlowModule] Loading account info for "
                        //           << cmd.cardNumber << std::endl;
                        newCmd = {CommandType::ATM_LOAD_ACCOUNT_INFO,
                                  "FlowModule",
                                  "ATM",
                                  cmd.cardNumber,
                                  0,
                                  0,
                                  NULL,
                                  cmd.callback};
                        commandQueue.push(newCmd);
                        break;

                    case CommandType::USER_SELECT_ACCOUNT:
                        // std::cout << "[FlowModule] Selecting account for "
                        //           << cmd.cardNumber << std::endl;
                        newCmd = {CommandType::USER_SELECT_ACCOUNT,
                                  "FlowModule",
                                  "ATM",
                                  cmd.cardNumber,
                                  0,
                                  0,
                                  NULL,
                                  cmd.callback,
                                  cmd.accountNumber};
                        commandQueue.push(newCmd);
                        break;
                    default:
                        break;
                }
            }
        }
    }
}
