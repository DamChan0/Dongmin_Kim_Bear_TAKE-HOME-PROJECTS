#include <type.h>

class ATMStatus {
   private:
    bool isATMOn;
    bool isCardInserted;
    bool isPinEntered;
    bool isAccountSelected;

   public:
    ATMStatus() : isATMOn(false), isCardInserted(false), isPinEntered(false), isAccountSelected(false) {}

    void setATMOn() { isATMOn = true; }

    void setATMOff() { isATMOn = false; }

    void setCard() { isCardInserted = true; }

    void removeCard() { isCardInserted = false; }

    void setPin() { isPinEntered = true; }

    void resetPin() { isPinEntered = false; }

    void setAccount() { isAccountSelected = true; }
    void resetAccount() { isAccountSelected = false; }

    bool getATMOnStatus() const { return isATMOn; }
    bool getCardInsertedStatus() const { return isCardInserted; }
    bool getPinEnteredStatus() const { return isPinEntered; }
    bool getAccountSelectedStatus() const { return isAccountSelected; }

    void checkStatus();
};