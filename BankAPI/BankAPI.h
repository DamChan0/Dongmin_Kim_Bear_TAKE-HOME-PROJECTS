#ifndef BANK_API_H
#define BANK_API_H

#include <fstream>
#include <map>
#include <mutex>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

class Account
{
   public:
    std::string accountHolder;
    std::string cardNumber;
    uint64_t pin;
    double balance;

    friend void to_json(nlohmann::json &j, const Account &account);
    friend void from_json(const nlohmann::json &j, Account &account);
};

class BankAPI
{
   private:
    static std::map<std::string, Account> accounts;  // 계좌 목록 (카드번호 → 계좌 정보)
    static std::mutex accountMutex;  // 멀티스레드 동시 접근 방지
    static constexpr const char *ACCOUNT_FILE = "accountList.json";

    static void saveAccounts();

   public:
    static void loadAccounts();
    static bool verifyPin(const std::string &cardNumber, uint64_t pin);
    static bool deposit(const std::string &cardNumber, double amount);
    static bool withdraw(const std::string &cardNumber, double amount);
    static double getBalance(const std::string &cardNumber);
};

using AccountList = std::map<std::string, Account>;

#endif  // BANK_API_H
