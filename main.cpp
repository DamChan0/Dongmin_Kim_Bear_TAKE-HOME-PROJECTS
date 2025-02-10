#include <ATMModule.h>
#include <type.h>
#include <user.h>

#include <fstream>
#include <iostream>

int main() {
    UserInfo user;
    user.insertCard("1234567890");

    uint64_t pinNum;
    std::cin >> pinNum;
    user.inuPutPin(pinNum);
}