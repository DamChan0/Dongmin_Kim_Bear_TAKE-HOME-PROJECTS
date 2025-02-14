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
  6. Exit

- ## temporary DB

    - ### account 1

        - Card Number: 1234567890
        - PIN: 1234
        - Balance: 1000
        - holder name: Dongmin Kim

    - ### account 2

        - Card Number: 0987654321
        - PIN: 4321
        - Balance: 2000
        - holder name: Baer Robotics

---

# SW Architecture Block Diagram

![atmStructure](https://github.com/user-attachments/assets/dea86ab3-000c-4bdb-bb1b-191dba5df081)
