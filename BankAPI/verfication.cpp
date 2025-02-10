#include <verification.h>

#include <unordered_map>

static std::unordered_map<std::string, uint64_t> pinDatabase = {
    {"1234567890", 1234},
    {"0987654321", 5678},
};

bool BankAPI::verifyPin(const std::string& cardNumber, uint64_t pinNum)
{
    auto it = pinDatabase.find(cardNumber);
    if (it != pinDatabase.end())
    {
        return it->second == pinNum;  // PIN이 일치하면 true 반환
    }
    return false;  // 카드 번호가 없거나 PIN이 틀릴 경우
}