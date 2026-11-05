// ======================================================================
// \title  stub_assert_hook.cpp
// \brief  Fw_AssertHook implementation with no formatting and no output
//
// For targets without a console or without stdio. Assertion failures
// still dispatch to a registered Fw::AssertHook; without one, the
// message is dropped and terminateOnAssert() is called.
// ======================================================================
#include <Fw/Types/assert_hook.hpp>
#include <cassert>

void Fw::formatAssertMessage(FILE_NAME_ARG file,
                             FwSizeType lineNo,
                             FwSizeType numArgs,
                             FwAssertArgType arg1,
                             FwAssertArgType arg2,
                             FwAssertArgType arg3,
                             FwAssertArgType arg4,
                             FwAssertArgType arg5,
                             FwAssertArgType arg6,
                             CHAR* destBuffer,
                             FwSizeType buffSize) {
    if ((destBuffer != nullptr) && (buffSize > 0)) {
        destBuffer[0] = '\0';
    }
}

void Fw::printAssertMessage(const CHAR* msg) {}

void Fw::terminateOnAssert() {
    assert(false);
}
