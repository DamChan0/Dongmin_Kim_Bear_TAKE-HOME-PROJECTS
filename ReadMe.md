# Dependency

- OS
  - Linux (Ubuntu 22.04)
- Package
  - nolhmann-json3-dev
    - `sudo apt install nlohmann-json3-dev`
- Language
  - C++ 17

# Build

``` bash
Cmake -S . -B build
Cmake --build build
```

# Run

``` bash
./build/ATMController
```

# Usage

Starting ATM is needed so press 1 to start ATM

- ## ATM Menu

  1. Insert Card
  2. Enter PIN
  3. Deposit
  4. Withdraw
  5. Check Balance
  6. Select Account
  7. RESET
  6. Exit

- ## temporary DB
  - accountLists.json
    key is Card Number
    value is Account Information
    ``` json
    {
      "1234": {
        "cardNumber": "1234",
        "pin": "1234",
        "accountNumber": "1234",
        "balance": 1000
      }
    }
    ```

---

# SW Architecture Block Diagram

![atmStructure](https://github.com/user-attachments/assets/dea86ab3-000c-4bdb-bb1b-191dba5df081)
