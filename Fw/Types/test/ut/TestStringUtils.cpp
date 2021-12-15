//
// Created by mstarch on 12/7/20.
//
#include <gtest/gtest.h>
#include <cstring>
#include <Fw/Types/StringUtils.hpp>

TEST(Nominal, string_copy) {
    const char* copy_string = "abc123\n";  // Length of 7
    char buffer_out_test[10];
    char buffer_out_truth[10];

    char* out_truth = ::strncpy(buffer_out_truth, copy_string, sizeof(buffer_out_truth));
    char* out_test = Fw::StringUtils::string_copy(buffer_out_test, copy_string, sizeof(buffer_out_test));

    ASSERT_EQ(sizeof(buffer_out_truth), sizeof(buffer_out_test)) << "Buffer size mismatch";

    // Check the outputs, both should return the input buffer
    ASSERT_EQ(out_truth, buffer_out_truth) << "strncpy didn't return expected value";
    ASSERT_EQ(out_test, buffer_out_test) << "string_copy didn't return expected value";

    // Check string correct
    ASSERT_STREQ(out_test, copy_string) << "Strings not equal from strncpy";
    ASSERT_STREQ(out_test, out_truth) << "Copied strings differ from strncpy";

    // Should output 0s for the remaining buffer
    for (U32 i = ::strnlen(buffer_out_truth, sizeof(buffer_out_truth)); i < static_cast<U32>(sizeof(buffer_out_truth));
         i++) {
        ASSERT_EQ(buffer_out_truth[i], 0) << "strncpy didn't output 0 fill";
        ASSERT_EQ(buffer_out_test[i], 0) << "string_copy didn't output 0 fill";
    }
}

TEST(OffNominal, string_copy) {
    const char* copy_string = "abc123\n";  // Length of 7
    char buffer_out_test[sizeof(copy_string) - 1];
    char buffer_out_truth[sizeof(copy_string) - 1];

    char* out_truth = ::strncpy(buffer_out_truth, copy_string, sizeof(buffer_out_truth));
    char* out_test = Fw::StringUtils::string_copy(buffer_out_test, copy_string, sizeof(buffer_out_test));

    ASSERT_EQ(sizeof(buffer_out_truth), sizeof(buffer_out_test)) << "Buffer size mismatch";

    // Check the outputs, both should return the input buffer
    ASSERT_EQ(out_truth, buffer_out_truth) << "strncpy didn't return expected value";
    ASSERT_EQ(out_test, buffer_out_test) << "string_copy didn't return expected value";

    // Check string correct up to last digit
    U32 i = 0;
    ASSERT_STRNE(out_test, out_truth) << "Strings not equal";
    for (i = 0; i < static_cast<U32>(sizeof(copy_string)) - 2; i++) {
        ASSERT_EQ(out_test[i], out_truth[i]);
    }
    ASSERT_EQ(out_truth[i], '\n') << "strncpy did not error as expected";
    ASSERT_EQ(out_test[i], 0) << "string_copy didn't properly null terminate";
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}