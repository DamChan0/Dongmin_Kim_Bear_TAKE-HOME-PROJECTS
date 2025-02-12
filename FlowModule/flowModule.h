#ifndef FLOWMODULE_H
#define FLOWMODULE_H
#include <ATMModule.h>
#include <commandQueue.h>
#include <user.h>

#include <thread>

enum class CommandType
{
    USER_INSERT_CARD,
    USER_VERIFY_PIN,
    USER_DEPOSIT,
    USER_WITHDRAW,
    ATM_ON,
    ATM_OFF,
    ATM_DEPOSIT,
    ATM_WITHDRAW,
    ATM_LOAD_ACCOUNT_INFO,
    ATM_CHECK_BALANCE,
    UNKNOWN,
};

struct Command
{
    CommandType type = CommandType::UNKNOWN;
    std::string TX = "";
    std::string RX = "";
    std::string cardNumber = "";
    uint64_t pin = 0;
    double amount = 0;
    std::any data = std::function<void(bool)>{};
};

class FlowModule
{
   private:
    bool running;
    std::thread flowThread;

    void processCommands();

   public:
    CommandQueue<Command> commandQueue;
    FlowModule();
    ~FlowModule();

    void addCommand(const Command& cmd);
};

#endif  // FLOWMODULE_H