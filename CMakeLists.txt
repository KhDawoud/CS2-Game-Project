cmake_minimum_required(VERSION 3.21)
project(QtTest)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(TestApp main.cpp player.cpp)

target_link_libraries(TestApp PRIVATE Qt6::Widgets)