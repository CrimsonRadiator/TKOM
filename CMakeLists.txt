cmake_minimum_required(VERSION 3.13)
project(TKOM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")

include_directories(include)
add_subdirectory(tests)
add_executable(TKOM src/main.cpp include/json.hpp src/Scanner.cpp include/Scanner.h include/SymbolTable.h src/Source.cpp include/Source.h src/Token.cpp include/Token.h )