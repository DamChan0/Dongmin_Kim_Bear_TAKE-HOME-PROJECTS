#include <ATMModule.h>
#include <BankAPI.h>
#include <flowModule.h>

#include <future>

ATMStatus::ATMStatus(FlowModule &fm) : flowModule(fm)
{
    ATMThread = std::thread(&ATMStatus::processATMStatus, this);
}

void ATMStatus::processATMStatus()
{
    while (true)
    {
        if (flowModule.commandQueue.empty())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        Command cmd = flowModule.commandQueue.pop();
        if (cmd.RX != "ATM")
        {
            flowModule.commandQueue.push(cmd);
        }
        Command newCmd;

        if (cmd.RX == "ATM")
        {
            switch (cmd.type)
            {
                case CommandType::ATM_ON:
                    setATMOn();
                    if (cmd.data.has_value())
                    {
                        auto promise = std::any_cast<std::promise<bool> *>(cmd.data);
                        promise->set_value(true);
                    }
                    break;

                case CommandType::ATM_OFF:
                    setATMOff();
                    std::cout << "[ATM] System is Terminating" << std::endl;
                    return;

                case CommandType::USER_INSERT_CARD:
                    if (getATMOnStatus())
                    {
                        setCard();
                        std::cout << "[ATM] Card inserted: " << cmd.cardNumber
                                  << std::endl;
                        cardNumber = cmd.cardNumber;
                    }
                    else
                    {
                        std::cout << "[ATM] ATM is off. Please turn on the ATM first."
                                  << std::endl;
                    }
                    break;

                case CommandType::USER_VERIFY_PIN:
                    if (getCardInsertedStatus())
                    {
                        setPin();
                        bool pinVerified = BankAPI::verifyPin(cmd.cardNumber, cmd.pin);

                        if (cmd.callback != nullptr)
                        {
                            if (pinVerified)
                            {
                                this->pinVerified = true;
                            }
                            cmd.callback(pinVerified);
                        }
                    }
                    else
                    {
                        std::cout << "[ATM] Please insert card first." << std::endl;
                    }
                    break;

                case CommandType::USER_DEPOSIT:
                    if (this->pinVerified)
                    {
                        std::cout << "[ATM] Depositing $" << cmd.amount << " to "
                                  << cmd.cardNumber << std::endl;
                        BankAPI::deposit(cmd.cardNumber, cmd.accountNumber, cmd.amount);
                        double balance =
                            BankAPI::getBalance(cmd.cardNumber, cmd.accountNumber);
                        std::cout << "[ATM]" << cmd.cardNumber << " balance: $" << balance
                                  << std::endl;
                    }
                    else
                    {
                        std::cout << "[ATM] Please verify your PIN first." << std::endl;
                    }
                    break;

                case CommandType::USER_WITHDRAW:
                    if (this->pinVerified)
                    {
                        std::cout << "[ATM] Withdrawing $" << cmd.amount << " from "
                                  << cmd.cardNumber << std::endl;
                        BankAPI::withdraw(cmd.cardNumber, cmd.accountNumber, cmd.amount);
                        double balance =
                            BankAPI::getBalance(cmd.cardNumber, cmd.accountNumber);
                        std::cout << "[ATM]" << cmd.cardNumber << " balance: $" << balance
                                  << std::endl;
                    }
                    else
                    {
                        std::cout << "[ATM] Please verify your PIN first." << std::endl;
                    }
                    break;

                case CommandType::USER_SELECT_ACCOUNT:
                    if (this->pinVerified)
                    {
                        bool checkAccount =
                            BankAPI::selectAccount(cmd.cardNumber, cmd.accountNumber);
                        if (checkAccount)
                        {
                            cmd.callback(cmd.accountNumber);
                            std::cout << "[ATM] Selecting account for " << cmd.cardNumber
                                      << std::endl;
                        }
                        else
                        {
                            cmd.callback(false);
                            std::cout << "[ATM] Account not found for " << cmd.cardNumber
                                      << std::endl;
                        }
                    }
                    else
                    {
                        std::cout << "[ATM] Please verify your PIN first." << std::endl;
                    }
                    break;

                case CommandType::ATM_LOAD_ACCOUNT_INFO:
                    if (getPinEnteredStatus())
                    {
                        std::cout << "[ATM] Loading account info for " << cmd.cardNumber
                                  << std::endl;
                        BankAPI::diplayAccount(cmd.cardNumber);
                    }
                    else
                    {
                        std::cout << "[ATM] Please verify your PIN first." << std::endl;
                    }
                    break;

                case CommandType::ATM_CHECK_BALANCE:
                    if (this->pinVerified)
                    {
                        double balance =
                            BankAPI::getBalance(cmd.cardNumber, cmd.accountNumber);
                        std::cout << "[ATM]" << "Card Number : " << cmd.cardNumber
                                  << " Account Number : " << cmd.accountNumber
                                  << " balance: $" << balance << std::endl;
                    }
                    else
                    {
                        std::cout << "[ATM] Please verify your PIN first." << std::endl;
                    }
                    break;
                case CommandType ::ATM_RESET_INFO:
                    resetPin();
                    resetAccount();
                    removeCard();
                    break;
                default:
                    break;
            }
        }
    }
}