cmake_minimum_required(VERSION 3.14)
project(hashbench)

set(CMAKE_CXX_STANDARD 17)

add_executable(hashbench hashbench.cpp)

target_link_libraries(hashbench -lpthread)
target_link_libraries(hashbench -lcryptopp)
