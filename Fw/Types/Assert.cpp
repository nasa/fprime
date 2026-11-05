#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/StringUtils.hpp>
#include <Fw/Types/assert_hook.hpp>
#include <cstdio>
#include <cstdlib>

namespace Fw {

void AssertHook::printAssert(const CHAR* msg) {
    printAssertMessage(msg);
}

void AssertHook::reportAssert(FILE_NAME_ARG file,
                              FwSizeType lineNo,
                              FwSizeType numArgs,
                              FwAssertArgType arg1,
                              FwAssertArgType arg2,
                              FwAssertArgType arg3,
                              FwAssertArgType arg4,
                              FwAssertArgType arg5,
                              FwAssertArgType arg6) {
    CHAR destBuffer[FW_ASSERT_TEXT_SIZE] = {0};
    formatAssertMessage(file, lineNo, numArgs, arg1, arg2, arg3, arg4, arg5, arg6, destBuffer,
                        static_cast<FwSizeType>(sizeof(destBuffer)));
    // print message
    this->printAssert(destBuffer);
}

void AssertHook::doAssert() {
    terminateOnAssert();
}

AssertHook* AssertHook::s_assertHook = nullptr;

void AssertHook::registerHook() {
    this->previousHook = s_assertHook;
    s_assertHook = this;
}

void AssertHook::deregisterHook() {
    s_assertHook = this->previousHook;
}

AssertHook* AssertHook::getRegisteredHook() {
    return s_assertHook;
}

// Default handler of SwAssert functions
FW_ASSERT_NORETURN void defaultSwAssert(FILE_NAME_ARG file,
                                        FwSizeType lineNo,
                                        FwSizeType numArgs,
                                        FwAssertArgType arg1,
                                        FwAssertArgType arg2,
                                        FwAssertArgType arg3,
                                        FwAssertArgType arg4,
                                        FwAssertArgType arg5,
                                        FwAssertArgType arg6) {
    Fw::AssertHook* const registeredHook = Fw::AssertHook::getRegisteredHook();
    if (nullptr == registeredHook) {
        CHAR assertMsg[FW_ASSERT_TEXT_SIZE];
        formatAssertMessage(file, lineNo, numArgs, arg1, arg2, arg3, arg4, arg5, arg6, assertMsg,
                            static_cast<FwSizeType>(sizeof(assertMsg)));
        printAssertMessage(assertMsg);
        terminateOnAssert();
    } else {
        registeredHook->reportAssert(file, lineNo, numArgs, arg1, arg2, arg3, arg4, arg5, arg6);
        registeredHook->doAssert();
    }
#if FW_ASSERTIONS_ALWAYS_ABORT
    abort();
#endif
}

void SwAssert(FILE_NAME_ARG file, FwSizeType lineNo) {
    defaultSwAssert(file, lineNo, 0, 0, 0, 0, 0, 0, 0);
}

void SwAssert(FILE_NAME_ARG file, FwAssertArgType arg1, FwSizeType lineNo) {
    defaultSwAssert(file, lineNo, 1, arg1, 0, 0, 0, 0, 0);
}

void SwAssert(FILE_NAME_ARG file, FwAssertArgType arg1, FwAssertArgType arg2, FwSizeType lineNo) {
    defaultSwAssert(file, lineNo, 2, arg1, arg2, 0, 0, 0, 0);
}

void SwAssert(FILE_NAME_ARG file, FwAssertArgType arg1, FwAssertArgType arg2, FwAssertArgType arg3, FwSizeType lineNo) {
    defaultSwAssert(file, lineNo, 3, arg1, arg2, arg3, 0, 0, 0);
}

void SwAssert(FILE_NAME_ARG file,
              FwAssertArgType arg1,
              FwAssertArgType arg2,
              FwAssertArgType arg3,
              FwAssertArgType arg4,
              FwSizeType lineNo) {
    defaultSwAssert(file, lineNo, 4, arg1, arg2, arg3, arg4, 0, 0);
}

void SwAssert(FILE_NAME_ARG file,
              FwAssertArgType arg1,
              FwAssertArgType arg2,
              FwAssertArgType arg3,
              FwAssertArgType arg4,
              FwAssertArgType arg5,
              FwSizeType lineNo) {
    defaultSwAssert(file, lineNo, 5, arg1, arg2, arg3, arg4, arg5, 0);
}

void SwAssert(FILE_NAME_ARG file,
              FwAssertArgType arg1,
              FwAssertArgType arg2,
              FwAssertArgType arg3,
              FwAssertArgType arg4,
              FwAssertArgType arg5,
              FwAssertArgType arg6,
              FwSizeType lineNo) {
    defaultSwAssert(file, lineNo, 6, arg1, arg2, arg3, arg4, arg5, arg6);
}
}  // namespace Fw

// define C asserts with C linkage
extern "C" {
I8 CAssert0(FILE_NAME_ARG file, FwSizeType lineNo);
I8 CAssert1(FILE_NAME_ARG file, FwAssertArgType arg1, FwSizeType lineNo);
I8 CAssert2(FILE_NAME_ARG file, FwAssertArgType arg1, FwAssertArgType arg2, FwSizeType lineNo);
I8 CAssert3(FILE_NAME_ARG file, FwAssertArgType arg1, FwAssertArgType arg2, FwAssertArgType arg3, FwSizeType lineNo);
I8 CAssert4(FILE_NAME_ARG file,
            FwAssertArgType arg1,
            FwAssertArgType arg2,
            FwAssertArgType arg3,
            FwAssertArgType arg4,
            FwSizeType lineNo);
I8 CAssert5(FILE_NAME_ARG file,
            FwAssertArgType arg1,
            FwAssertArgType arg2,
            FwAssertArgType arg3,
            FwAssertArgType arg4,
            FwAssertArgType arg5,
            FwSizeType lineNo);
I8 CAssert6(FILE_NAME_ARG file,
            FwAssertArgType arg1,
            FwAssertArgType arg2,
            FwAssertArgType arg3,
            FwAssertArgType arg4,
            FwAssertArgType arg5,
            FwAssertArgType arg6,
            FwSizeType lineNo);
}

I8 CAssert0(FILE_NAME_ARG file, FwSizeType lineNo) {
    Fw::AssertHook* const registeredHook = Fw::AssertHook::getRegisteredHook();
    if (nullptr == registeredHook) {
        CHAR assertMsg[FW_ASSERT_TEXT_SIZE];
        Fw::formatAssertMessage(file, lineNo, 0, 0, 0, 0, 0, 0, 0, assertMsg,
                                static_cast<FwSizeType>(sizeof(assertMsg)));
        Fw::printAssertMessage(assertMsg);
        Fw::terminateOnAssert();
    } else {
        registeredHook->reportAssert(file, lineNo, 0, 0, 0, 0, 0, 0, 0);
        registeredHook->doAssert();
    }
    return 0;
}

I8 CAssert1(FILE_NAME_ARG file, FwAssertArgType arg1, FwSizeType lineNo) {
    Fw::AssertHook* const registeredHook = Fw::AssertHook::getRegisteredHook();
    if (nullptr == registeredHook) {
        CHAR assertMsg[FW_ASSERT_TEXT_SIZE];
        Fw::formatAssertMessage(file, lineNo, 1, arg1, 0, 0, 0, 0, 0, assertMsg,
                                static_cast<FwSizeType>(sizeof(assertMsg)));
        Fw::printAssertMessage(assertMsg);
        Fw::terminateOnAssert();
    } else {
        registeredHook->reportAssert(file, lineNo, 1, arg1, 0, 0, 0, 0, 0);
        registeredHook->doAssert();
    }
    return 0;
}

I8 CAssert2(FILE_NAME_ARG file, FwAssertArgType arg1, FwAssertArgType arg2, FwSizeType lineNo) {
    Fw::AssertHook* const registeredHook = Fw::AssertHook::getRegisteredHook();
    if (nullptr == registeredHook) {
        CHAR assertMsg[FW_ASSERT_TEXT_SIZE];
        Fw::formatAssertMessage(file, lineNo, 2, arg1, arg2, 0, 0, 0, 0, assertMsg,
                                static_cast<FwSizeType>(sizeof(assertMsg)));
        Fw::printAssertMessage(assertMsg);
        Fw::terminateOnAssert();
    } else {
        registeredHook->reportAssert(file, lineNo, 2, arg1, arg2, 0, 0, 0, 0);
        registeredHook->doAssert();
    }
    return 0;
}

I8 CAssert3(FILE_NAME_ARG file, FwAssertArgType arg1, FwAssertArgType arg2, FwAssertArgType arg3, FwSizeType lineNo) {
    Fw::AssertHook* const registeredHook = Fw::AssertHook::getRegisteredHook();
    if (nullptr == registeredHook) {
        CHAR assertMsg[FW_ASSERT_TEXT_SIZE];
        Fw::formatAssertMessage(file, lineNo, 3, arg1, arg2, arg3, 0, 0, 0, assertMsg,
                                static_cast<FwSizeType>(sizeof(assertMsg)));
        Fw::printAssertMessage(assertMsg);
        Fw::terminateOnAssert();
    } else {
        registeredHook->reportAssert(file, lineNo, 3, arg1, arg2, arg3, 0, 0, 0);
        registeredHook->doAssert();
    }
    return 0;
}

I8 CAssert4(FILE_NAME_ARG file,
            FwAssertArgType arg1,
            FwAssertArgType arg2,
            FwAssertArgType arg3,
            FwAssertArgType arg4,
            FwSizeType lineNo) {
    Fw::AssertHook* const registeredHook = Fw::AssertHook::getRegisteredHook();
    if (nullptr == registeredHook) {
        CHAR assertMsg[FW_ASSERT_TEXT_SIZE];
        Fw::formatAssertMessage(file, lineNo, 4, arg1, arg2, arg3, arg4, 0, 0, assertMsg,
                                static_cast<FwSizeType>(sizeof(assertMsg)));
        Fw::printAssertMessage(assertMsg);
        Fw::terminateOnAssert();
    } else {
        registeredHook->reportAssert(file, lineNo, 4, arg1, arg2, arg3, arg4, 0, 0);
        registeredHook->doAssert();
    }
    return 0;
}

I8 CAssert5(FILE_NAME_ARG file,
            FwAssertArgType arg1,
            FwAssertArgType arg2,
            FwAssertArgType arg3,
            FwAssertArgType arg4,
            FwAssertArgType arg5,
            FwSizeType lineNo) {
    Fw::AssertHook* const registeredHook = Fw::AssertHook::getRegisteredHook();
    if (nullptr == registeredHook) {
        CHAR assertMsg[FW_ASSERT_TEXT_SIZE];
        Fw::formatAssertMessage(file, lineNo, 5, arg1, arg2, arg3, arg4, arg5, 0, assertMsg,
                                static_cast<FwSizeType>(sizeof(assertMsg)));
        Fw::printAssertMessage(assertMsg);
        Fw::terminateOnAssert();
    } else {
        registeredHook->reportAssert(file, lineNo, 5, arg1, arg2, arg3, arg4, arg5, 0);
        registeredHook->doAssert();
    }
    return 0;
}

I8 CAssert6(FILE_NAME_ARG file,
            FwAssertArgType arg1,
            FwAssertArgType arg2,
            FwAssertArgType arg3,
            FwAssertArgType arg4,
            FwAssertArgType arg5,
            FwAssertArgType arg6,
            FwSizeType lineNo) {
    Fw::AssertHook* const registeredHook = Fw::AssertHook::getRegisteredHook();
    if (nullptr == registeredHook) {
        CHAR assertMsg[FW_ASSERT_TEXT_SIZE];
        Fw::formatAssertMessage(file, lineNo, 6, arg1, arg2, arg3, arg4, arg5, arg6, assertMsg,
                                static_cast<FwSizeType>(sizeof(assertMsg)));
        Fw::printAssertMessage(assertMsg);
        Fw::terminateOnAssert();
    } else {
        registeredHook->reportAssert(file, lineNo, 6, arg1, arg2, arg3, arg4, arg5, arg6);
        registeredHook->doAssert();
    }
    return 0;
}
