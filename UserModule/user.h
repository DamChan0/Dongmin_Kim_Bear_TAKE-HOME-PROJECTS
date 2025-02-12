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
    bool running = true;
    bool pinVerified = false;

    void processUserInput();  // 사용자 입력을 처리하는 스레드 함수

   public:
    User(FlowModule& fm);
    ~User();
};

#endif  // USER_H
