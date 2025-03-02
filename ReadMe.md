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





# Patch 

- 2025.03.02 patch <span style="color:red;">[critical]</span>
  - ubuntu 24.04 / g++ (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0 컴파일 환경의 경우 #include<any>를 포함하지 않아도 빌드가 가능했지만 22.04이하의 버전에서는 명시적 include 없이는 빌드가 불가능한 문제가 있음 그와 별개로 사용하는 std library는 명시하는 것이 옳기 때문에 수정하였음
    - patch 내용
      - <span style="color:green;">+++ #include<any></span>
--- 