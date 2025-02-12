#include <ATMModule.h>
#include <BankAPI.h>
#include <flowModule.h>

#include <future>

ATMStatus::ATMStatus(FlowModule &fm)
    : isATMOn(false),
      isCardInserted(false),
      isPinEntered(false),
      isAccountSelected(false),
      flowModule(fm)
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
        std::cout << "[ATM] Command received: " << cmd.RX << std::endl;
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
                    break;

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
                        if (cmd.data.has_value())  // ✅ 값이 있을 때만 실행
                        {
                            try
                            {
                                auto callback =
                                    std::any_cast<std::function<void(bool)>>(cmd.data);
                                if (callback)  // ✅ 콜백이 유효한지 확인
                                {
                                    callback(pinVerified);  // ✅ User에게 결과 반환
                                }
                            }
                            catch (const std::bad_any_cast &e)
                            {
                                std::cerr << "[ERROR] std::any_cast failed: " << e.what()
                                          << std::endl;
                            }
                        }
                        break;
                    }
                    else
                    {
                        std::cout << "[ATM] Please insert card first." << std::endl;
                    }
                    break;

                case CommandType::USER_DEPOSIT:
                    if (getPinEnteredStatus())
                    {
                        std::cout << "[ATM] Depositing $" << cmd.amount << " to "
                                  << cmd.cardNumber << std::endl;
                    }
                    else
                    {
                        std::cout << "[ATM] Please verify your PIN first." << std::endl;
                    }
                    break;

                case CommandType::USER_WITHDRAW:
                    if (getPinEnteredStatus())
                    {
                        std::cout << "[ATM] Withdrawing $" << cmd.amount << " from "
                                  << cmd.cardNumber << std::endl;
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
                    }
                    else
                    {
                        std::cout << "[ATM] Please verify your PIN first." << std::endl;
                    }
                    break;

                case CommandType::ATM_CHECK_BALANCE:
                    if (getPinEnteredStatus())
                    {
                        std::cout << "[ATM] Checking balance for " << cmd.cardNumber
                                  << std::endl;
                    }
                    else
                    {
                        std::cout << "[ATM] Please verify your PIN first." << std::endl;
                    }
                    break;
                default:
                    break;
            }
        }
    }
}