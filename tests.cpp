#include <gtest/gtest.h>

TEST(test_basic, basic_test_set)
{
    testing::internal::CaptureStdout();

    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_TRUE(true);
}


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}