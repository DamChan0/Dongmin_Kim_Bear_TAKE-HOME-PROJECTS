#include <string>
#include <vector>

typedef struct Account
{
    std::string accountNumber;
    double balance;
} Account_t;

class ATMStatus
{
  private:
    bool isATMOn;
    bool isCardInserted;
    bool isPinEntered;
    bool isAccountSelected;

  public:
    ATMStatus() : isATMOn(false), isCardInserted(false), isPinEntered(false), isAccountSelected(false)
    {
    }
};

class UserInfo
{
  private:
    std::string cardNumber;
    uint64_t pinPassword;
    std::vector<Account_t> accounts;

  public:
};