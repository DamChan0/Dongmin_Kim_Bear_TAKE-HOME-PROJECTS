#ifndef ATMMODULE_H
#define ATMMODULE_H

#include <type.h>

#include <thread>

class FlowModule;

class ATMStatus
{
   public:
    ATMStatus(FlowModule& fm);
    ~ATMStatus()
    {
        if (ATMThread.joinable())
        {
            ATMThread.join();
        }
    }

    FlowModule& flowModule;
    void processATMStatus();
    bool isATMOff;

   private:
    bool isATMOn;
    bool isCardInserted;
    bool isPinEntered;
    bool isAccountSelected;

    std::string cardNumber;
    bool pinVerified;

    std::thread ATMThread;

    void setATMOn() { isATMOn = true; }

    void setATMOff() { isATMOff = false; }

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

#endif  // ATMMODULE_H