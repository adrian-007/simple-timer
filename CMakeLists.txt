cmake_minimum_required(VERSION 3.10)

project(simple_timer CXX)

add_library(simple_timer
    src/simple_timer.cpp
)

target_include_directories(simple_timer PUBLIC include)

add_subdirectory(test)
