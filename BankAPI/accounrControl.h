#define ACCOUNTCONTROL_H
#ifdef ACCOUNTCONTROL_H
#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

class Account {
   public:
    std::string accountHolder;  // 계좌 소유자
    double balance;             // 잔액
    std::string accountType;    // 계좌 유형

    // JSON 변환을 위한 함수 선언
    friend void to_json(nlohmann::json &j, const Account &account);
    friend void from_json(const nlohmann::json &j, Account &account);
};

using AccountList = std::map<std::string, Account>;

void to_json(nlohmann::json &j, const Account &account) {
    j = {
        {"accountHolder", account.accountHolder},
        {"balance", account.balance},
    };
}

void from_json(const nlohmann::json &j, Account &account) {
    j.at("accountHolder").get_to(account.accountHolder);
    j.at("balance").get_to(account.balance);
}

#endif  // ACCOUNTCONTROL_H