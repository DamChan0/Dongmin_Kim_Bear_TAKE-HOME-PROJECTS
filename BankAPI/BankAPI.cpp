#include <BankAPI.h>

#include <iostream>
#include <unordered_map>

using njson = nlohmann::json;
std::map<std::string, Account> BankAPI::accounts;
std::mutex BankAPI::accountMutex;

std::string BankAPI::getAccountFilePath()
{
    return std::filesystem::current_path().string() + "/accountList.json";
}

void to_json(nlohmann::json& j, const Account& account)
{
    j = {{"accountHolder", account.accountHolder},
         {"cardNumber", account.cardNumber},
         {"pin", account.pin},
         {"balance", account.balance}};
}

void from_json(const nlohmann::json& j, Account& account)
{
    j.at("accountHolder").get_to(account.accountHolder);
    j.at("cardNumber").get_to(account.cardNumber);
    j.at("pin").get_to(account.pin);
    j.at("balance").get_to(account.balance);
}

bool BankAPI::verifyPin(const std::string& cardNumber, uint64_t pinNum)
{
    loadAccounts();
    std::lock_guard<std::mutex> lock(accountMutex);
    if (accounts.find(cardNumber) != accounts.end())
    {
        std::cout << "[BankAPI] Account is found." << std::endl;

        if (accounts[cardNumber].pin != pinNum)
        {
            std::cout << "[BankAPI] Incorrect PIN." << std::endl;
            return false;
        }
        else
        {
            std::cout << "[BankAPI] Correct PIN." << std::endl;
            return true;
        }
        return false;
    }
    else
    {
        std::cout << "[BankAPI] Account not found." << std::endl;
    }
    return (pinNum == 1234);  // 더미 PIN 검증
}

bool BankAPI::withdraw(const std::string& cardNumber, double amount)
{
    std::unique_lock<std::mutex> lock(accountMutex);

    if (amount <= 0 || accounts[cardNumber].balance < amount)
    {
        std::cout
            << "[BankAPI_ERROR] Withdrawal failed. Insufficient funds or invalid amount."
            << std::endl;
        return false;
    }
    accounts[cardNumber].balance -= amount;
    lock.unlock();
    saveAccount(cardNumber);
    std::cout << "[Bank] Withdrawn $" << amount << " from " << cardNumber
              << ". New Balance: $" << accounts[cardNumber].balance << std::endl;
    return true;
}

bool BankAPI::deposit(const std::string& cardNumber, double amount)
{
    std::unique_lock<std::mutex> lock(accountMutex);
    if (amount <= 0)
    {
        std::cout << "[BankAPI_ERROR] Invalid deposit amount." << std::endl;
        return false;
    }
    accounts[cardNumber].balance += amount;
    lock.unlock();
    saveAccount(cardNumber);
    std::cout << "[Bank] Deposited $" << amount << " to " << cardNumber
              << ". New Balance: $" << accounts[cardNumber].balance << std::endl;
    return true;
}

double BankAPI::getBalance(const std::string& cardNumber)
{
    std::lock_guard<std::mutex> lock(accountMutex);
    return accounts[cardNumber].balance;
}

void BankAPI::loadAccounts()
{
    std::lock_guard<std::mutex> lock(accountMutex);
    std::string ACCOUNT_FILE = getAccountFilePath();
    std::fstream file(ACCOUNT_FILE, std::ios::in);
    if (!file.is_open())
    {
        std::cerr << "[BankAPI_ERROR] Failed to open account database." << std::endl;
        return;
    }

    njson jsonData;
    file >> jsonData;
    file.close();

    accounts.clear();  // 기존 데이터 초기화

    for (auto& item : jsonData.items())
    {
        Account account;
        account.cardNumber = item.key();
        from_json(item.value(), account);
        accounts[account.cardNumber] = account;
    }

    std::cout << "[Bank] Loaded " << accounts.size() << " accounts." << std::endl;
}

void BankAPI::saveAccount(const std::string& cardNumber)
{
    std::lock_guard<std::mutex> lock(accountMutex);
    std::string ACCOUNT_FILE = getAccountFilePath();

    std::ifstream inFile(ACCOUNT_FILE);
    nlohmann::json jsonData;
    if (inFile.is_open())
    {
        inFile >> jsonData;
        inFile.close();
    }

    if (accounts.find(cardNumber) != accounts.end())
    {
        jsonData[cardNumber] = accounts[cardNumber];
    }
    else
    {
        std::cerr << "[BankAPI] Account not found: " << cardNumber << std::endl;
        return;
    }

    std::ofstream outFile(ACCOUNT_FILE);
    if (!outFile.is_open())
    {
        std::cerr << "[BankAPI] Failed to save account database." << std::endl;
        return;
    }
    outFile << jsonData.dump(4);
    outFile.close();
}
