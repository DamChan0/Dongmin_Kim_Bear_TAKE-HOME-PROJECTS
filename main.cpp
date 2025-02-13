#include <ATMModule.h>
#include <flowModule.h>
#include <type.h>
#include <user.h>

#include <fstream>
#include <iostream>

int main()
{
    FlowModule flowModule;

    User user(flowModule);
    ATMStatus atmStatus(flowModule);

    return 0;
}