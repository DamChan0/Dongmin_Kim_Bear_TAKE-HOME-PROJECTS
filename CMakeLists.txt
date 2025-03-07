cmake_minimum_required(VERSION 3.10)

# 프로젝트 이름
project(ATMController)

# C++ 표준 설정
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 컴파일 옵션 설정
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror ")

# Debug 모드에서 추가 플래그 설정
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

# 헤더 파일 경로 설정
include_directories("${CMAKE_SOURCE_DIR}/BankAPI"
"${CMAKE_SOURCE_DIR}/ATMModule"
"${CMAKE_SOURCE_DIR}/UserModule"
"${CMAKE_SOURCE_DIR}/include"
"${CMAKE_SOURCE_DIR}/FlowModule"
)


# 소스 파일 추가
file(GLOB SRC_FILES
    "${CMAKE_SOURCE_DIR}/BankAPI/*.cpp"
    "${CMAKE_SOURCE_DIR}/ATMModule/*.cpp"
    "${CMAKE_SOURCE_DIR}/FlowModule/*.cpp"
    "${CMAKE_SOURCE_DIR}/UserModule/*.cpp"
    "${CMAKE_SOURCE_DIR}/main.cpp"
)
add_executable(ATMController ${SRC_FILES})

# accountList.json 파일을 빌드 출력 디렉토리에 복사
add_custom_command(TARGET ATMController POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/accountList.json"
    $<TARGET_FILE_DIR:ATMController>/accountList.json
)