cmake_minimum_required(VERSION 3.10)

# 프로젝트 이름
project(ATMController)

# C++ 표준 설정
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 컴파일 옵션 설정
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wno-unused-function -Wno-unused-variable")

# Debug 모드에서 추가 플래그 설정
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

# 헤더 파일 경로 설정
include_directories("${CMAKE_SOURCE_DIR}/BankAPI")

# 소스 파일 추가
file(GLOB_RECURSE SRC_FILES "${CMAKE_SOURCE_DIR}/*.cpp")
add_executable(ATMController ${SRC_FILES})
