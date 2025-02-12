#include <BankAPI.h>

#include <iostream>
#include <unordered_map>

using njson = nlohmann::json;
std::map<std::string, Account> BankAPI::accounts;
std::mutex BankAPI::accountMutex;

void to_json(nlohmann::json& j, const Account& account)
{
    j = {{"accountHolder", account.accountHolder}, {"balance", account.balance}};
}

void from_json(const nlohmann::json& j, Account& account)
{
    j.at("accountHolder").get_to(account.accountHolder);
    j.at("balance").get_to(account.balance);
}

bool BankAPI::verifyPin(const std::string& cardNumber, uint64_t pinNum)
{
    std::cout << "[BankAPI] Verifying PIN for card: " << cardNumber << std::endl;
    return (pinNum == 1234);  // 더미 PIN 검증
}

bool BankAPI::withdraw(const std::string& cardNumber, double amount)
{
    std::lock_guard<std::mutex> lock(accountMutex);
    if (amount <= 0 || accounts[cardNumber].balance < amount)
    {
        std::cout << "[BankAPI] Withdrawal failed. Insufficient funds or invalid amount."
                  << std::endl;
        return false;
    }
    accounts[cardNumber].balance -= amount;
    saveAccounts();
    std::cout << "[BankAPI] Withdrawn $" << amount << " from " << cardNumber
              << ". New Balance: $" << accounts[cardNumber].balance << std::endl;
    return true;
}

bool BankAPI::deposit(const std::string& cardNumber, double amount)
{
    std::lock_guard<std::mutex> lock(accountMutex);
    if (amount <= 0)
    {
        std::cout << "[BankAPI] Invalid deposit amount." << std::endl;
        return false;
    }
    accounts[cardNumber].balance += amount;
    saveAccounts();
    std::cout << "[BankAPI] Deposited $" << amount << " to " << cardNumber
              << ". New Balance: $" << accounts[cardNumber].balance << std::endl;
    return true;
}

void BankAPI::loadAccounts()
{
    std::lock_guard<std::mutex> lock(accountMutex);
    std::fstream file(ACCOUNT_FILE, std::ios::in);
    if (!file.is_open())
    {
        std::cerr << "[BankAPI] Failed to open account database." << std::endl;
        return;
    }

    njson jsonData;
    file >> jsonData;
    file.close();

    for (auto& item : jsonData.items())
    {
        std::string cardNumber = item.key();
        Account account = item.value().get<Account>();
        accounts[cardNumber] = account;
    }

    std::cout << "[BankAPI] Loaded " << accounts.size() << " accounts." << std::endl;

    for (const auto& [cardNumber, account] : accounts)
    {
        std::cout << "[BankAPI] Account: " << cardNumber
                  << ", Balance: " << account.balance << std::endl;
    }
}

void BankAPI::saveAccounts()
{
    std::lock_guard<std::mutex> lock(accountMutex);
    nlohmann::json jsonData;

    for (const auto& accountPair : accounts)
    {
        jsonData[accountPair.first] = accountPair.second;
    }

    std::ofstream file(ACCOUNT_FILE);
    if (!file.is_open())
    {
        std::cerr << "[BankAPI] Failed to save account database." << std::endl;
        return;
    }
    file << jsonData.dump(4);
    file.close();
}
