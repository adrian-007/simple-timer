add_executable(testrunner
    tests.cpp
)

target_link_libraries(testrunner
    simple_timer
    gtest
    gtest_main
    gmock
    gmock_main
)
