cmake_minimum_required(VERSION 3.7)
project(tgen)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")

include_directories(include)
add_subdirectory(tests)
add_executable(tgen src/main.cpp include/json.h src/Scanner.cpp include/Scanner.h include/CodeGenerator.h src/Source.cpp include/Source.h src/Token.cpp include/Token.h src/Parser.cpp include/Parser.h src/Node.cpp include/Node.h src/Logger.cpp include/Logger.h src/CodeGenerator.cpp src/JsonDeserializer.cpp include/JsonDeserializer.h)