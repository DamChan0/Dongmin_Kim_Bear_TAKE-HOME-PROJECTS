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
    ATM_DEPOSIT,
    ATM_WITHDRAW,
    ATM_CHECK_BALANCE,
};

struct Command
{
    CommandType type;
    std::string cardNumber;
    uint64_t pin;
    double amount;
};

class FlowModule
{
   private:
    bool running;
    CommandQueue<Command> userCommandQueue;  // User → ATM 명령 전달용 큐
    CommandQueue<Command> bankCommandQueue;  // ATM → BankAPI 명령 전달용 큐
    std::thread flowThread;

    void processCommands();

   public:
    FlowModule();
    ~FlowModule();

    void addUserCommand(const Command& cmd);
    void addBankCommand(const Command& cmd);
};

#endif  // FLOWMODULE_H