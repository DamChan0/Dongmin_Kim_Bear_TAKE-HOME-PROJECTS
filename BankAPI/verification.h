#ifndef VERIFICATION_H
#define VERIFICATION_H
#include <cstdint>
#include <string>

class BankAPI
{
   public:
    BankAPI() {}
    // PIN 검증 메서드 (cardNumber와 입력된 PIN 비교)
    static bool verifyPin(const std::string& cardNumber, uint64_t pinNum);
};

#endif  // VERIFICATION_H
