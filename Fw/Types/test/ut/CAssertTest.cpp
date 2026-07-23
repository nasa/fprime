#include <Fw/Types/CAssert.h>
#include <gtest/gtest.h>
#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/Assert.hpp>

/**
 * \file CAssertTest.cpp
 * \brief Tests for FW_CASSERT_1 macro functionality
 *
 * Note: F' C assertions use an assert hook system rather than directly
 * terminating the program. This allows for configurable assert handling
 * in embedded systems.
 */

namespace {
// Define an Assert handler
class TestAssertHook : public Fw::AssertHook {
  public:
    TestAssertHook() {}
    virtual ~TestAssertHook() {}
    void reportAssert(FILE_NAME_ARG file,
                      FwSizeType lineNo,
                      FwSizeType numArgs,
                      FwAssertArgType arg1,
                      FwAssertArgType arg2,
                      FwAssertArgType arg3,
                      FwAssertArgType arg4,
                      FwAssertArgType arg5,
                      FwAssertArgType arg6) {
        this->m_file = file;
        this->m_lineNo = lineNo;
        this->m_numArgs = numArgs;
        this->m_arg1 = arg1;
        this->m_arg2 = arg2;
        this->m_arg3 = arg3;
        this->m_arg4 = arg4;
        this->m_arg5 = arg5;
        this->m_arg6 = arg6;
    };

    void doAssert() { this->m_asserted = true; }

    FILE_NAME_ARG getFile() { return this->m_file; }

    FwSizeType getLineNo() { return this->m_lineNo; }

    FwSizeType getNumArgs() { return this->m_numArgs; }

    FwAssertArgType getArg1() { return this->m_arg1; }

    FwAssertArgType getArg2() { return this->m_arg2; }

    FwAssertArgType getArg3() { return this->m_arg3; }

    FwAssertArgType getArg4() { return this->m_arg4; }

    FwAssertArgType getArg5() { return this->m_arg5; }

    FwAssertArgType getArg6() { return this->m_arg6; }

    bool asserted() {
        bool didAssert = this->m_asserted;
        this->m_asserted = false;
        return didAssert;
    }

  private:
#if FW_ASSERT_LEVEL == FW_FILEID_ASSERT
    // Setting this to a non-zero initially as the test
    // should set it to 0.
    FILE_NAME_ARG m_file = 1;
#else
    FILE_NAME_ARG m_file = nullptr;
#endif
    FwSizeType m_lineNo = 0;
    FwSizeType m_numArgs = 0;
    FwAssertArgType m_arg1 = 0;
    FwAssertArgType m_arg2 = 0;
    FwAssertArgType m_arg3 = 0;
    FwAssertArgType m_arg4 = 0;
    FwAssertArgType m_arg5 = 0;
    FwAssertArgType m_arg6 = 0;
    bool m_asserted = false;
};
}  // namespace

// Test that FW_CASSERT_1 macro compiles and works correctly when true
TEST(CAssertTest, CAssert1Macro) {
// Disable old-style cast warnings for this test since we're testing C macros
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"

    // Test that the macro compiles without errors
    int testValue = 42;

    // This should not trigger an assertion since condition is true
    FW_CASSERT_1(testValue == 42, testValue);

    // Test with different argument types
    U32 uintValue = 100;
    FW_CASSERT_1(uintValue > 0, uintValue);

    I32 intValue = -1;
    FW_CASSERT_1(intValue < 0, intValue);

#pragma GCC diagnostic pop

    // Verify that the macro expands correctly
    EXPECT_TRUE(true);  // If we get here, the macro compiled successfully
}

// Test basic FW_CASSERT macro (no arguments)
TEST(CAssertTest, CAssertMacro) {
// Disable old-style cast warnings for this test since we're testing C macros
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"

    // Test basic assertion with true condition
    FW_CASSERT(1 == 1);
    FW_CASSERT(true);

#pragma GCC diagnostic pop

    // If we reach here, compilation and execution were successful
    EXPECT_TRUE(true);
}

// Test that FW_CASSERT_1 properly triggers assert hook when condition is false
TEST(CAssertTest, CAssert1MacroFailure) {
#if FW_ASSERT_LEVEL != FW_NO_ASSERT
// Disable old-style cast warnings for this test since we're testing C macros
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"

    // Create and register test assert hook
    TestAssertHook hook;
    hook.registerHook();

    // Test FW_CASSERT_1 with failing condition
    int testValue = 42;
    FW_CASSERT_1(testValue == 999, testValue);  // This should trigger the assert hook

    // Verify that assertion was triggered
    EXPECT_TRUE(hook.asserted());
    EXPECT_EQ(hook.getNumArgs(), 1U);
    EXPECT_EQ(hook.getArg1(), static_cast<FwAssertArgType>(testValue));

    // Deregister hook
    hook.deregisterHook();

#pragma GCC diagnostic pop
#else
    GTEST_SKIP() << "Assertions are disabled (FW_ASSERT_LEVEL == FW_NO_ASSERT), skipping assert failure test";
#endif
}

// Test that the multi-argument FW_CASSERT_N macros forward all their arguments to the hook
TEST(CAssertTest, CAssertMultiArgMacroFailure) {
#if FW_ASSERT_LEVEL != FW_NO_ASSERT
// Disable old-style cast warnings for this test since we're testing C macros
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"

    TestAssertHook hook;
    hook.registerHook();

    // FW_CASSERT_2
    FW_CASSERT_2(false, 1, 2);
    EXPECT_TRUE(hook.asserted());
    EXPECT_EQ(hook.getNumArgs(), 2U);
    EXPECT_EQ(hook.getArg1(), static_cast<FwAssertArgType>(1));
    EXPECT_EQ(hook.getArg2(), static_cast<FwAssertArgType>(2));

    // FW_CASSERT_3
    FW_CASSERT_3(false, 1, 2, 3);
    EXPECT_TRUE(hook.asserted());
    EXPECT_EQ(hook.getNumArgs(), 3U);
    EXPECT_EQ(hook.getArg3(), static_cast<FwAssertArgType>(3));

    // FW_CASSERT_4
    FW_CASSERT_4(false, 1, 2, 3, 4);
    EXPECT_TRUE(hook.asserted());
    EXPECT_EQ(hook.getNumArgs(), 4U);
    EXPECT_EQ(hook.getArg4(), static_cast<FwAssertArgType>(4));

    // FW_CASSERT_5
    FW_CASSERT_5(false, 1, 2, 3, 4, 5);
    EXPECT_TRUE(hook.asserted());
    EXPECT_EQ(hook.getNumArgs(), 5U);
    EXPECT_EQ(hook.getArg5(), static_cast<FwAssertArgType>(5));

    // FW_CASSERT_6
    FW_CASSERT_6(false, 1, 2, 3, 4, 5, 6);
    EXPECT_TRUE(hook.asserted());
    EXPECT_EQ(hook.getNumArgs(), 6U);
    EXPECT_EQ(hook.getArg1(), static_cast<FwAssertArgType>(1));
    EXPECT_EQ(hook.getArg2(), static_cast<FwAssertArgType>(2));
    EXPECT_EQ(hook.getArg3(), static_cast<FwAssertArgType>(3));
    EXPECT_EQ(hook.getArg4(), static_cast<FwAssertArgType>(4));
    EXPECT_EQ(hook.getArg5(), static_cast<FwAssertArgType>(5));
    EXPECT_EQ(hook.getArg6(), static_cast<FwAssertArgType>(6));

    hook.deregisterHook();

#pragma GCC diagnostic pop
#else
    GTEST_SKIP() << "Assertions are disabled (FW_ASSERT_LEVEL == FW_NO_ASSERT), skipping assert failure test";
#endif
}

// Test that FW_CASSERT properly triggers assert hook when condition is false
TEST(CAssertTest, CAssertMacroFailure) {
#if FW_ASSERT_LEVEL != FW_NO_ASSERT
// Disable old-style cast warnings for this test since we're testing C macros
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"

    // Create and register test assert hook
    TestAssertHook hook;
    hook.registerHook();

    // Test FW_CASSERT with failing condition
    FW_CASSERT(1 == 2);  // This should trigger the assert hook

    // Verify that assertion was triggered
    EXPECT_TRUE(hook.asserted());
    EXPECT_EQ(hook.getNumArgs(), 0U);  // FW_CASSERT has no arguments

    // Deregister hook
    hook.deregisterHook();

#pragma GCC diagnostic pop
#else
    GTEST_SKIP() << "Assertions are disabled (FW_ASSERT_LEVEL == FW_NO_ASSERT), skipping assert failure test";
#endif
}
