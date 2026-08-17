// ======================================================================
// \title  TestAssertHook.hpp
// \brief  Shared assert hook for Fw/Types unit tests
// ======================================================================

#ifndef FW_TYPES_TEST_ASSERT_HOOK_HPP
#define FW_TYPES_TEST_ASSERT_HOOK_HPP

#include <Fw/Types/Assert.hpp>

namespace Fw {

//! Assert hook that records the last reported assert for test inspection
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
                      FwAssertArgType arg6) override {
        this->m_file = file;
        this->m_lineNo = lineNo;
        this->m_numArgs = numArgs;
        this->m_arg1 = arg1;
        this->m_arg2 = arg2;
        this->m_arg3 = arg3;
        this->m_arg4 = arg4;
        this->m_arg5 = arg5;
        this->m_arg6 = arg6;
    }

    void doAssert() override { this->m_asserted = true; }

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
    // Non-zero initial value; tests may check that an assert sets it
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

}  // namespace Fw

#endif
