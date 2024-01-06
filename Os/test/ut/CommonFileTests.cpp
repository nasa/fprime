#include <gtest/gtest.h>
#include "Os/File.hpp"
#include "Os/test/ut/CommonFileTests.hpp"

// Ensure that open and close mode changes work reliably
TEST(BaseFunctionality, ModeTransitions) {
    Os::File file;
    ASSERT_EQ(file.mode, Os::File::Mode::OPEN_NO_MODE);
    test_open_helper(file, "/tmp/fprime-test");
    ASSERT_NE(file.mode, Os::File::Mode::OPEN_NO_MODE);
    file.close();
    ASSERT_EQ(file.mode, Os::File::Mode::OPEN_NO_MODE);
}

// Ensure that isOpen works reliably
TEST(BaseFunctionality, IsOpenFunctionality) {
    Os::File file;
    ASSERT_FALSE(file.isOpen());
    test_open_helper(file, "/tmp/fprime-test");
    ASSERT_TRUE(file.isOpen());
    file.close();
    ASSERT_FALSE(file.isOpen());
}

// Ensure that Os::File properly refuses preallocate calls when not open
TEST(BaseFunctionality, PreallocateWithoutOpen) {
    Os::File file;
    ASSERT_EQ(file.preallocate(0, 10), Os::File::Status::NOT_OPENED);
}
