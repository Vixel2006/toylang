cmake_minimum_required(VERSION 3.23)

project(toylang)

set(CMAKE_CXX_STANDARD 17)

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
add_definitions(-DLLVM_DISABLE_ABI_BREAKING_CHECKS_ENFORCING=1)

add_executable(toylang src/main.cpp src/lexer.cpp)

target_include_directories(toylang PRIVATE include)


