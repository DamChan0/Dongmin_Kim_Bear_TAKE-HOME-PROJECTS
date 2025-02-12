#include <ATMModule.h>
#include <flowModule.h>
#include <type.h>
#include <user.h>

#include <fstream>
#include <iostream>

int main()
{
    FlowModule flowModule;
    std::cout << "ATM is ON\n";

    User user(flowModule);
    ATMStatus atmStatus(flowModule);

    // FlowModule의 명령 처리 스레드 시작
    // std::thread commandThread(&FlowModule::processCommands, &flowModule);

    // 프로그램을 종료하지 않고 유지
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));  // CPU 사용률 방지
    }

    // 프로그램이 종료될 때까지 대기

    return 0;
}