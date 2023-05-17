cmake_minimum_required(VERSION 3.25)
project(can)

set(CMAKE_CXX_STANDARD 17)

add_executable(tx tx.cpp driver_can.cpp)
target_link_libraries(tx pthread)

add_executable(rx rx.cpp driver_can.cpp)
target_link_libraries(rx pthread)
