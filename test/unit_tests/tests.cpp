#include <iostream>

#include <gtest/gtest.h>
#include <simple_timer.hpp>

TEST(TestCase, TestName)
{
    ASSERT_TRUE(true);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
