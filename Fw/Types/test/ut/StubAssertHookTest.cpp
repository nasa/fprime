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

TEST(StubAssertHook, FormatYieldsEmptyMessage) {
    CHAR buffer[FW_ASSERT_TEXT_SIZE];
    buffer[0] = 'x';
#if FW_ASSERT_LEVEL == FW_FILEID_ASSERT
    FILE_NAME_ARG file = 0x1234;
#else
    FILE_NAME_ARG file = "StubAssertHookTest.cpp";
#endif
    Fw::formatAssertMessage(file, 42, 2, 1, 2, 0, 0, 0, 0, buffer, static_cast<FwSizeType>(sizeof(buffer)));
    EXPECT_STREQ(buffer, "");
    Fw::printAssertMessage(buffer);
}

TEST(StubAssertHook, FormatToleratesEmptyBuffer) {
#if FW_ASSERT_LEVEL == FW_FILEID_ASSERT
    FILE_NAME_ARG file = 0x1234;
#else
    FILE_NAME_ARG file = "StubAssertHookTest.cpp";
#endif
    Fw::formatAssertMessage(file, 1, 0, 0, 0, 0, 0, 0, 0, nullptr, 0);
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

TEST(StubAssertHookDeathTest, TerminateAsserts) {
    EXPECT_DEATH(Fw::terminateOnAssert(), "");
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
