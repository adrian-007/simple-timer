cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 11)

project(simple_timer CXX)

add_library(simple_timer
    src/simple_timer.cpp
)

target_include_directories(simple_timer PUBLIC include)

if (NOT WIN32)
    target_link_libraries(simple_timer pthread)
endif()

add_subdirectory(test)
