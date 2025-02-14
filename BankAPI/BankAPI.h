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
    std::string accountNumber;
    std::string accountHolder;
    std::string cardNumber;
    uint64_t pin;
    double balance;

    friend void to_json(nlohmann::json &j, const std::vector<Account> &accounts);

    friend void from_json(const nlohmann::json &j, std::vector<Account> &accounts);
};

class BankAPI
{
   private:
    static std::map<std::string, std::vector<Account>> accounts;
    static std::mutex accountMutex;

    static void saveAccount(const std::string &cardNumber, uint64_t accountNumber);

   public:
    static void loadAccounts();
    static std::string getAccountFilePath();
    static void diplayAccount(const std::string &cardNumber);
    static bool selectAccount(const std::string &cardNumber, uint64_t accountNumber);

    static bool verifyPin(const std::string &cardNumber, uint64_t pin);
    static bool deposit(const std::string &cardNumber, uint64_t accountNumber,
                        double amount);
    static bool withdraw(const std::string &cardNumber, uint64_t accountNumber,
                         double amount);
    static double getBalance(const std::string &cardNumber, uint64_t accountNumber);
};

using AccountList = std::map<std::string, Account>;

#endif  // BANK_API_H
