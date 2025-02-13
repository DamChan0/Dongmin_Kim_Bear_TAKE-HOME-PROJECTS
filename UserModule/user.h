#ifndef USER_H
#define USER_H

#include <atomic>
#include <iostream>
#include <thread>
class FlowModule;

class User
{
   private:
    FlowModule& flowModule;
    std::thread userThread;
    std::string cardNumber;
    bool cardInserted = false;
    bool pinVerified = false;
    bool accountSelected = false;
    bool isWaiting = false;

    void onPinVerificationResult(bool& pinVerified);
    void onCheckBalance(double balance);

    void userInput();

   public:
    bool running = false;
    User(FlowModule& fm);
    ~User()
    {
        running = false;
        if (userThread.joinable())
        {
            userThread.join();
        }
    }
};

#endif  // USER_H
