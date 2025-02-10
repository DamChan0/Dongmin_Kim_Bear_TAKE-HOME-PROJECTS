#define USER_H
#ifdef USER_H
#include <ATMModule.h>
#include <accounrControl.h>
#include <type.h>

#include <fstream>
#include <iostream>

class UserInfo
{
   private:
    std::string cardNumber;
    uint64_t pinNum;
    bool isPinverified;
    std::vector<Account> accounts;
    ATMStatus atmStatus;

   public:
    UserInfo() : cardNumber(""), pinNum(0) {}

    uint32_t inuPutPin(uint64_t pinPassword);
    bool verifyPin(uint64_t pinPassword);
    void insertCard(std::string cardNumber);
    void getAccountInfo(std::string cardNumnber, bool isPinverified);
    void seleteAccount();
    void deposit(double amount);
    void withdraw(double amount);
};
#endif  // USER_H