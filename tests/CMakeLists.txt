cmake_minimum_required(VERSION 3.7)
project(TKOMtests)

set(CMAKE_CXX_STANDARD 17)
#boost
enable_testing()
find_package(Boost COMPONENTS unit_test_framework REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

file(GLOB TKOMsrc ${CMAKE_SOURCE_DIR}/include/*.h ${CMAKE_SOURCE_DIR}/src/*.cpp)
list(REMOVE_ITEM TKOMsrc "${CMAKE_SOURCE_DIR}/src/main.cpp")

# creates the executable
add_executable(tkom_tests TKOMtests.cpp ${TKOMsrc} )

# for code coverage
SET(GCC_COVERAGE_COMPILE_FLAGS "-fprofile-arcs -ftest-coverage")
SET(GCC_COVERAGE_LINK_FLAGS "-lgcov")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

target_include_directories(tkom_tests PRIVATE ${Boost_INCLUDE_DIRS} PUBLIC ../include/)
# declares a test with our executable
add_test(NAME TKOMtests COMMAND tkom_tests)