#include <gtest/gtest.h>
#include <config/FpConfig.hpp>

/**
 * \file AssertTypesTest.h
 * \author Vince Woo
 * \brief Tests for FW_ASSERT_LEVEL fall back when ASSERT_FILE_ID or
 *        ASSERT_RELATIVE_PATH are missing.
 */

#if FW_ASSERT_LEVEL == FW_FILEID_ASSERT
#undef ASSERT_FILE_ID
#elif FW_ASSERT_LEVEL == FW_RELATIVE_PATH_ASSERT
#undef ASSERT_RELATIVE_PATH
#endif

#include <Fw/Types/Assert.hpp>

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
    };

    void doAssert() { this->m_asserted = true; }

    FILE_NAME_ARG getFile() { return this->m_file; }

    FwSizeType getLineNo() { return this->m_lineNo; }

    FwSizeType getNumArgs() { return this->m_numArgs; }

    FwAssertArgType getArg1() { return this->m_arg1; }

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
    bool m_asserted = false;
};

//
TEST(AssertTypesTest, FileDefaultTest) {
    // register the class
    TestAssertHook hook;
    hook.registerHook();

    // issue an assert
    FW_ASSERT(0);
    // hook should have intercepted it
    ASSERT_TRUE(hook.asserted());

#if FW_ASSERT_LEVEL == FW_FILEID_ASSERT
    // ASSERT_FILE_ID was undefined above, it should have defaulted to 0
    ASSERT_EQ(0, hook.getFile());
#elif FW_ASSERT_LEVEL == FW_RELATIVE_PATH_ASSERT
    // ASSERT_RELATIVE_PATH was undefined above, it should have defaulted
    // to the full path
    ASSERT_EQ(__FILE__, hook.getFile());
#endif
}
