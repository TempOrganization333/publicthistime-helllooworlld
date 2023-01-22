cmake_minimum_required(VERSION 3.14)
project(Two) #optional

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 17)

# add_executable(two two.cpp) #same as g++ two.cpp -o Two

# For GoogleTest
include_directories(..)
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

enable_testing()

add_executable (testing test/two_test.cpp)
target_link_libraries(testing gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(testing)