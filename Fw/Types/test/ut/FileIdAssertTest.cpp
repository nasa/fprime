#include <FpConfig.hpp>
#include <gtest/gtest.h>

#undef ASSERT_FILE_ID

#undef FW_ASSERT_LEVEL
#define FW_ASSERT_LEVEL FW_FILEID_ASSERT

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
        FILE_NAME_ARG m_file = 0;
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

TEST(RelativePathAssertTest, CheckAssertTest) {
    // register the class
    TestAssertHook hook;
    hook.registerHook();

    // issue an assert
    FW_ASSERT(0==1);
    // hook should have intercepted it
    ASSERT_TRUE(hook.asserted());
    ASSERT_EQ(0u, hook.getFile());
}
