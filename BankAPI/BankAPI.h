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
    static std::map<std::string, Account> accounts;
    static std::mutex accountMutex;

    static void saveAccount(const std::string &cardNumber);

   public:
    static void loadAccounts();
    static std::string getAccountFilePath();

    static bool verifyPin(const std::string &cardNumber, uint64_t pin);
    static bool deposit(const std::string &cardNumber, double amount);
    static bool withdraw(const std::string &cardNumber, double amount);
    static double getBalance(const std::string &cardNumber);
};

using AccountList = std::map<std::string, Account>;

#endif  // BANK_API_H
