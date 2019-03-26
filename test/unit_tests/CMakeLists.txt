add_executable(unit_tests
    tests.cpp
)

target_link_libraries(unit_tests
    simple_timer
    gtest
    gtest_main
    gmock
    gmock_main
)
