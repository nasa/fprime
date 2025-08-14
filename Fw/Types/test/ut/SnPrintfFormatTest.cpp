#include <gtest/gtest.h>
#include <cstdio>
#include "Fw/Types/format.hpp"

TEST(Nominal, snprintf_format) {
    char buffer_test[100];
    char buffer_real[100];
    const char* test_format_string = "Hello %s";
    const char* test_string = "World";

    Fw::FormatStatus status = Fw::stringFormat(buffer_test, sizeof(buffer_test), test_format_string, test_string);
    EXPECT_EQ(status, Fw::FormatStatus::SUCCESS);
    snprintf(buffer_real, sizeof(buffer_real), test_format_string, test_string);
    EXPECT_STREQ(buffer_test, buffer_real);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
