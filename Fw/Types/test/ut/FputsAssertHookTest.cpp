#include <gtest/gtest.h>
#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/assert_hook.hpp>

// Built with CHOOSES_IMPLEMENTATIONS Fw_AssertHook_fputs: checks the formatted text for
// every argument count and that printing goes to stderr.

namespace {
#if FW_ASSERT_LEVEL == FW_FILEID_ASSERT
FILE_NAME_ARG TEST_FILE = 0x1234;
const char* const EXPECTED_PREFIX = "Assert: 0x00001234:42";
#else
FILE_NAME_ARG TEST_FILE = "FputsAssertHookTest.cpp";
const char* const EXPECTED_PREFIX = "Assert: \"FputsAssertHookTest.cpp:42\"";
#endif

const char* const EXPECTED_TAILS[] = {"", " 1", " 1 2", " 1 2 3", " 1 2 3 4", " 1 2 3 4 5", " 1 2 3 4 5 6"};
}  // namespace

TEST(FputsAssertHook, FormatsEveryArgumentCount) {
    for (FwSizeType numArgs = 0; numArgs <= 6; numArgs++) {
        CHAR buffer[FW_ASSERT_TEXT_SIZE] = {0};
        Fw::formatAssertMessage(TEST_FILE, 42, numArgs, 1, 2, 3, 4, 5, 6, buffer,
                                static_cast<FwSizeType>(sizeof(buffer)));
        std::string expected(EXPECTED_PREFIX);
        expected += EXPECTED_TAILS[numArgs];
        EXPECT_STREQ(buffer, expected.c_str()) << "numArgs=" << numArgs;
    }
}

TEST(FputsAssertHook, FormatYieldsEmptyMessageOnBadArgumentCount) {
    CHAR buffer[FW_ASSERT_TEXT_SIZE] = {'x', 0};
    Fw::formatAssertMessage(TEST_FILE, 42, 7, 1, 2, 3, 4, 5, 6, buffer, static_cast<FwSizeType>(sizeof(buffer)));
    EXPECT_STREQ(buffer, "");
}

TEST(FputsAssertHook, PrintWritesLineToStderr) {
    ::testing::internal::CaptureStderr();
    Fw::printAssertMessage("hello assert");
    EXPECT_EQ(::testing::internal::GetCapturedStderr(), "hello assert\n");
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
