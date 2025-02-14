#include <BankAPI.h>
#include <unistd.h>

#include <iostream>
#include <unordered_map>

#define PATH_MAX 4096

using njson = nlohmann::json;

std::map<std::string, std::vector<Account>> BankAPI::accounts;
std::mutex BankAPI::accountMutex;

std::string getExecutableDir()
{
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    std::string fullPath = (count != -1) ? std::string(result, count) : "";
    return std::filesystem::path(fullPath).parent_path().string();
}

std::string BankAPI::getAccountFilePath()
{
    return getExecutableDir() + "/accountList.json";
}

void to_json(nlohmann::json& j, const std::vector<Account>& accounts)
{
    j = nlohmann::json::array();  // JSON 배열로 초기화

    for (const auto& account : accounts)
    {
        nlohmann::json accountJson;
        accountJson["accountNumber"] = account.accountNumber;
        accountJson["accountHolder"] = account.accountHolder;
        accountJson["pin"] = account.pin;
        accountJson["balance"] = account.balance;

        j.push_back(accountJson);  // 배열에 추가
    }
}

void from_json(const nlohmann::json& j, std::vector<Account>& accounts)
{
    accounts.clear();   // 기존 데이터 초기화
    if (!j.is_array())  // JSON 데이터가 배열인지 확인
    {
        std::cerr << "[ERROR] Invalid JSON format: Expected an array." << std::endl;
        return;
    }

    for (const auto& item : j)
    {
        Account account;
        if (item.contains("accountNumber"))
            item.at("accountNumber").get_to(account.accountNumber);
        if (item.contains("accountHolder"))
            item.at("accountHolder").get_to(account.accountHolder);
        if (item.contains("pin")) item.at("pin").get_to(account.pin);
        if (item.contains("balance")) item.at("balance").get_to(account.balance);

        accounts.push_back(account);
    }
}

bool BankAPI::verifyPin(const std::string& cardNumber, uint64_t pinNum)
{
    loadAccounts();  // 최신 데이터 로드
    std::lock_guard<std::mutex> lock(accountMutex);

    // 카드 번호 존재 확인
    if (accounts.find(cardNumber) == accounts.end())
    {
        std::cout << "[BankAPI] Account not found for card: " << cardNumber << std::endl;
        return false;
    }

    std::cout << "[BankAPI] Card number found: " << cardNumber << std::endl;

    // 해당 카드 번호의 계좌 목록에서 PIN 확인
    for (const auto& account : accounts[cardNumber])
    {
        if (account.pin == pinNum)
        {
            std::cout << "[BankAPI] PIN Verified for account: " << account.accountNumber
                      << std::endl;
            return true;
        }
    }

    std::cout << "[BankAPI] Incorrect PIN for card: " << cardNumber << std::endl;
    return false;
}
bool BankAPI::withdraw(const std::string& cardNumber, uint64_t accountNumber,
                       double amount)
{
    std::unique_lock<std::mutex> lock(accountMutex);

    for (auto& account : accounts[cardNumber])
    {
        if (amount <= 0 || account.balance < amount)
        {
            std::cout << "[BankAPI_ERROR] Withdrawal failed. Insufficient funds or "
                         "invalid amount."
                      << std::endl;
            return false;
        }

        if (account.accountNumber == std::to_string(accountNumber))
        {
            account.balance -= amount;
            lock.unlock();
            saveAccount(cardNumber, accountNumber);
            std::cout << "[Bank] Withdrawn $" << amount << " from " << cardNumber
                      << ". New Balance: $" << account.balance << std::endl;
            return true;
        }
    }
    std::cout << "[BankAPI_ERROR] Account number not found." << std::endl;
    return false;
}

bool BankAPI::deposit(const std::string& cardNumber, uint64_t accountNumber,
                      double amount)
{
    std::unique_lock<std::mutex> lock(accountMutex);

    for (auto& account : accounts[cardNumber])
    {
        if (amount <= 0 || account.balance <= 0)
        {
            std::cout << "[BankAPI_ERROR] Deposit failed. Invalid amount." << std::endl;
            return false;
        }

        if (account.accountNumber == std::to_string(accountNumber))
        {
            account.balance += amount;
            lock.unlock();
            saveAccount(cardNumber, accountNumber);
            std::cout << "[Bank] Deposited $" << amount << " to " << cardNumber
                      << ". New Balance: $" << account.balance << std::endl;

            return true;
        }
    }
    std::cout << "[BankAPI_ERROR] Account number not found." << std::endl;
    return false;
}

double BankAPI::getBalance(const std::string& cardNumber, uint64_t accountNumber)
{
    std::string accountNumberStr = std::to_string(accountNumber);
    std::lock_guard<std::mutex> lock(accountMutex);
    if (accounts.find(cardNumber) == accounts.end())
    {
        std::cerr << "[BankAPI] Card number not found: " << cardNumber << std::endl;
    }

    for (const auto& account : accounts[cardNumber])
    {
        if (account.accountNumber == accountNumberStr)
        {
            return account.balance;
        }
    }
    return false;
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
        std::string cardNumber = item.key();
        std::vector<Account> accountList;
        from_json(item.value(), accountList);  // JSON 데이터를 벡터로 변환
        accounts[cardNumber] = accountList;
    }

    std::cout << "[Bank] Loaded " << accounts.size() << " card accounts." << std::endl;
}

void BankAPI::saveAccount(const std::string& cardNumber, uint64_t accountNumber)
{
    std::string accountNumberStr = std::to_string(accountNumber);
    std::lock_guard<std::mutex> lock(accountMutex);
    std::string ACCOUNT_FILE = getAccountFilePath();

    // 기존 JSON 데이터 로드
    std::ifstream inFile(ACCOUNT_FILE);
    nlohmann::json jsonData;
    if (inFile.is_open())
    {
        inFile >> jsonData;
        inFile.close();
    }

    if (accounts.find(cardNumber) == accounts.end())
    {
        std::cerr << "[BankAPI] Account not found for card: " << cardNumber << std::endl;
        return;
    }

    bool found = false;

    // 계좌 목록 업데이트
    for (auto& account : accounts[cardNumber])
    {
        if (account.accountNumber == accountNumberStr)
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        std::cerr << "[BankAPI] No matching account found for update." << std::endl;
        return;
    }

    // 계좌 정보를 JSON 변환
    nlohmann::json updatedAccountsJson;
    to_json(updatedAccountsJson, accounts[cardNumber]);  // ✅ 기존 `to_json` 호출

    // JSON 데이터 갱신
    jsonData[cardNumber] = updatedAccountsJson;

    // 변경된 JSON 데이터 저장
    std::ofstream outFile(ACCOUNT_FILE);
    if (!outFile.is_open())
    {
        std::cerr << "[BankAPI] Failed to save account database." << std::endl;
        return;
    }
    outFile << jsonData.dump(4);
    outFile.close();
}

void BankAPI::diplayAccount(const std::string& cardNumber)
{
    std::lock_guard<std::mutex> lock(accountMutex);

    for (auto& [key, value] : accounts)
    {
        if (key == cardNumber)
        {
            std::cout << "[BankAPI] Account found: " << key << std::endl;
            std::cout << "------------------" << std::endl;
            std::cout << "Account Number | Account Holder | Balance" << std::endl;
            std::cout << "------------------" << std::endl;
            for (const auto& account : value)
            {
                std::cout << account.accountNumber << " | " << account.accountHolder
                          << " | " << account.balance << std::endl;
            }
        }
    }
}

bool BankAPI::selectAccount(const std::string& cardNumber, uint64_t accountNumber)
{
    std::lock_guard<std::mutex> lock(accountMutex);
    std::string accountNumberStr = std::to_string(accountNumber);

    // 카드 번호 존재 확인
    if (accounts.find(cardNumber) == accounts.end())
    {
        std::cerr << "[BankAPI] Account not found for card: " << cardNumber << std::endl;
        return false;
    }

    // 카드 번호에 해당하는 계좌 목록에서 계좌 번호 확인
    for (const auto& account : accounts[cardNumber])
    {
        if (account.accountNumber == accountNumberStr)
        {
            std::cout << "[BankAPI] Account selected: " << accountNumber
                      << " (Holder: " << account.accountHolder << ")" << std::endl;
            return true;
        }
    }

    std::cerr << "[BankAPI] Account number " << accountNumber
              << " not found for card: " << cardNumber << std::endl;
    return false;
}
