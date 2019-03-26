find_package(GTest REQUIRED)

add_executable(unit_tests
    tests.cpp
)

target_link_libraries(unit_tests simple_timer)
target_link_libraries(unit_tests GTest::GTest)
