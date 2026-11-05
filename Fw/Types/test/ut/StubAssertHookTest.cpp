#include <gtest/gtest.h>
#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/assert_hook.hpp>

// Built with CHOOSES_IMPLEMENTATIONS Fw_AssertHook_Stub: the link-time default
// produces no message, and a registered Fw::AssertHook still receives asserts.

namespace {
class RecordingHook : public Fw::AssertHook {
  public:
    void reportAssert(FILE_NAME_ARG file,
                      FwSizeType lineNo,
                      FwSizeType numArgs,
                      FwAssertArgType arg1,
                      FwAssertArgType arg2,
                      FwAssertArgType arg3,
                      FwAssertArgType arg4,
                      FwAssertArgType arg5,
                      FwAssertArgType arg6) override {
        this->m_lineNo = lineNo;
        this->m_numArgs = numArgs;
        this->m_arg1 = arg1;
    }
    void doAssert() override { this->m_asserted = true; }

    FwSizeType m_lineNo = 0;
    FwSizeType m_numArgs = 0;
    FwAssertArgType m_arg1 = 0;
    bool m_asserted = false;
};
}  // namespace

#if FW_ASSERT_LEVEL == FW_FILEID_ASSERT
FILE_NAME_ARG TEST_FILE = 0x1234;
#else
FILE_NAME_ARG TEST_FILE = "StubAssertHookTest.cpp";
#endif

TEST(StubAssertHook, FormatYieldsEmptyMessage) {
    CHAR buffer[FW_ASSERT_TEXT_SIZE] = {0};
    buffer[0] = 'x';
    Fw::formatAssertMessage(TEST_FILE, 42, 2, 1, 2, 0, 0, 0, 0, buffer, static_cast<FwSizeType>(sizeof(buffer)));
    EXPECT_STREQ(buffer, "");
}

TEST(StubAssertHook, FormatToleratesEmptyBuffer) {
    CHAR sentinel[1] = {'x'};
    Fw::formatAssertMessage(TEST_FILE, 1, 0, 0, 0, 0, 0, 0, 0, sentinel, 0);
    EXPECT_EQ(sentinel[0], 'x');
    // nullptr with buffSize == 0 must not dereference; a crash fails the test
    Fw::formatAssertMessage(TEST_FILE, 1, 0, 0, 0, 0, 0, 0, 0, nullptr, 0);
}

TEST(StubAssertHook, PrintWritesNothing) {
    ::testing::internal::CaptureStderr();
    Fw::printAssertMessage("should not appear");
    EXPECT_EQ(::testing::internal::GetCapturedStderr(), "");
}

TEST(StubAssertHook, RegisteredHookStillDispatched) {
    RecordingHook hook;
    hook.registerHook();
    const FwSizeType line = static_cast<FwSizeType>(__LINE__) + 1;
    FW_ASSERT(false, 7);
    hook.deregisterHook();
    EXPECT_TRUE(hook.m_asserted);
    EXPECT_EQ(hook.m_lineNo, line);
    EXPECT_EQ(hook.m_numArgs, 1);
    EXPECT_EQ(hook.m_arg1, 7);
}

// terminateOnAssert() is assert(false), which compiles out under NDEBUG
#ifndef NDEBUG
TEST(StubAssertHookDeathTest, TerminateAsserts) {
    EXPECT_DEATH(Fw::terminateOnAssert(), "false");
}

TEST(StubAssertHookDeathTest, DefaultPathPrintsNothingAndDies) {
    // No hook registered: FW_ASSERT goes through the stub, so the first line on stderr is the
    // C library's own assertion message from terminateOnAssert(), not an F Prime "Assert:" line
    EXPECT_DEATH(FW_ASSERT(false, 7), "^[^\n]*terminateOnAssert");
}
#endif

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
