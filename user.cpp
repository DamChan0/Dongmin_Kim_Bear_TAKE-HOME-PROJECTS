#include <user.h>
#include <verification.h>

#include <nlohmann/json.hpp>
using njson = nlohmann::json;

void UserInfo::insertCard(std::string cardNumber)
{
    this->cardNumber = cardNumber;
    this->atmStatus.setCard();
    std::cout << "Card inserted: " << cardNumber << std::endl;
}

bool UserInfo::verifyPin(uint64_t pinNum)
{
    BankAPI bankAPI;
    // TODO: make verfication logic
    bool ret = bankAPI.verifyPin(this->cardNumber, pinNum);
    this->isPinverified = ret;
    return ret;
}

uint32_t UserInfo::inuPutPin(uint64_t pinNum)
{
    this->pinNum = pinNum;
    this->atmStatus.setPin();
    std::cout << "Pin entered: " << pinNum << std::endl;
    bool ret = UserInfo::verifyPin(pinNum);
    if (ret)
    {
        std::cout << "Pin verified" << std::endl;
        return 1;
    }
    else
    {
        std::cout << "Pin not verified" << std::endl;
        return 0;
    }
    return 0;
}

void UserInfo::seleteAccount()
{
    uint16_t closeSelection = 0;
    std::ifstream acoountList("accountList.json");
    if (!acoountList.is_open())
    {
        std::cerr << "Failed to open JSON file." << std::endl;
        return;
    }

    njson jsonData;

    acoountList >> jsonData;

    AccountList accounts = jsonData.get<AccountList>();
    njson newJsonData = accounts;

    std::cout << "(AccountList):\n";
    for (const auto &[cardNumber, account] : accounts)
    {
        std::cout << "Card Number: " << cardNumber << std::endl;
        std::cout << "  Account Holder: " << account.accountHolder << std::endl;
        std::cout << "  Balance: " << account.balance << std::endl;
        std::cout << "  Account Type: " << account.accountType << std::endl;
    }

    while (closeSelection != 1)
    {
        std::cout << "Please select an account" << std::endl;
        std::cout << "Type 'close' to close the selection" << std::endl;
        std::cout << "select account: ";
        std::string accountNumber;
        std::cin >> accountNumber;

        if (accountNumber == "close")
        {
            closeSelection = 1;
            break;
        }

        for (auto &account : accounts)
        {
            if (account.first == accountNumber)
            {
                this->accounts.push_back(account.second);
                std::cout << "Account selected: " << accountNumber << std::endl;
            }
            else
            {
                std::cout << "Account not found" << std::endl;
            }
        }
    }

    std::cout << "\nObject to JSON:\n" << newJsonData.dump(4) << std::endl;
}

void UserInfo::getAccountInfo(std::string cardNumnber, bool verified)
{
    if (verified)
    {
        seleteAccount();
    }
    else
    {
        std::cout << "Please verify your pin first" << std::endl;
    }
}