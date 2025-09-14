#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/StringUtils.hpp>
#include <Fw/Types/format.hpp>
#include <cassert>
#include <cstdio>

#if FW_ASSERT_LEVEL == FW_FILEID_ASSERT
#define fileIdFs "Assert: 0x%08" PRIx32 ":%" PRI_FwSizeType ""
#else
#define fileIdFs "Assert: \"%s:%" PRI_FwSizeType "\""
#endif

namespace Fw {

void defaultPrintAssert(const CHAR* msg) {
    // Write to stderr w/o formatting
    (void)fputs(msg, stderr);
    (void)fputs("\n", stderr);
}

void defaultReportAssert(FILE_NAME_ARG file,
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
    switch (numArgs) {
        case 0:
            (void)stringFormat(destBuffer, buffSize, fileIdFs, file, lineNo);
            break;
        case 1:
            (void)stringFormat(destBuffer, buffSize, fileIdFs " %" PRI_FwAssertArgType, file, lineNo, arg1);
            break;
        case 2:
            (void)stringFormat(destBuffer, buffSize, fileIdFs " %" PRI_FwAssertArgType " %" PRI_FwAssertArgType, file,
                               lineNo, arg1, arg2);
            break;
        case 3:
            (void)stringFormat(destBuffer, buffSize,
                               fileIdFs " %" PRI_FwAssertArgType " %" PRI_FwAssertArgType " %" PRI_FwAssertArgType,
                               file, lineNo, arg1, arg2, arg3);
            break;
        case 4:
            (void)stringFormat(destBuffer, buffSize,
                               fileIdFs " %" PRI_FwAssertArgType " %" PRI_FwAssertArgType " %" PRI_FwAssertArgType
                                        " %" PRI_FwAssertArgType,
                               file, lineNo, arg1, arg2, arg3, arg4);
            break;
        case 5:
            (void)stringFormat(destBuffer, buffSize,
                               fileIdFs " %" PRI_FwAssertArgType " %" PRI_FwAssertArgType " %" PRI_FwAssertArgType
                                        " %" PRI_FwAssertArgType " %" PRI_FwAssertArgType,
                               file, lineNo, arg1, arg2, arg3, arg4, arg5);
            break;
        case 6:
            (void)stringFormat(destBuffer, buffSize,
                               fileIdFs " %" PRI_FwAssertArgType " %" PRI_FwAssertArgType " %" PRI_FwAssertArgType
                                        " %" PRI_FwAssertArgType " %" PRI_FwAssertArgType " %" PRI_FwAssertArgType,
                               file, lineNo, arg1, arg2, arg3, arg4, arg5, arg6);
            break;
        default:  // in an assert already, what can we do?
            break;
    }
}

void AssertHook::printAssert(const CHAR* msg) {
    defaultPrintAssert(msg);
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
    CHAR destBuffer[FW_ASSERT_TEXT_SIZE];
    defaultReportAssert(file, lineNo, numArgs, arg1, arg2, arg3, arg4, arg5, arg6, destBuffer,
                        static_cast<FwSizeType>(sizeof(destBuffer)));
    // print message
    this->printAssert(destBuffer);
}

void AssertHook::doAssert() {
    assert(0);
}

static AssertHook* s_assertHook = nullptr;

void AssertHook::registerHook() {
    this->previousHook = s_assertHook;
    s_assertHook = this;
}

void AssertHook::deregisterHook() {
    s_assertHook = this->previousHook;
}

// Default handler of SwAssert functions
I8 defaultSwAssert(FILE_NAME_ARG file,
                   FwSizeType lineNo,
                   FwSizeType numArgs,
                   FwAssertArgType arg1,
                   FwAssertArgType arg2,
                   FwAssertArgType arg3,
                   FwAssertArgType arg4,
                   FwAssertArgType arg5,
                   FwAssertArgType arg6) {
    if (nullptr == s_assertHook) {
        CHAR assertMsg[FW_ASSERT_TEXT_SIZE];
        defaultReportAssert(file, lineNo, numArgs, arg1, arg2, arg3, arg4, arg5, arg6, assertMsg,
                            static_cast<FwSizeType>(sizeof(assertMsg)));
        defaultPrintAssert(assertMsg);
        assert(0);
    } else {
        s_assertHook->reportAssert(file, lineNo, numArgs, arg1, arg2, arg3, arg4, arg5, arg6);
        s_assertHook->doAssert();
    }
    return 0;
}

I8 SwAssert(FILE_NAME_ARG file, FwSizeType lineNo) {
    return defaultSwAssert(file, lineNo, 0, 0, 0, 0, 0, 0, 0);
}

I8 SwAssert(FILE_NAME_ARG file, FwAssertArgType arg1, FwSizeType lineNo) {
    return defaultSwAssert(file, lineNo, 1, arg1, 0, 0, 0, 0, 0);
}

I8 SwAssert(FILE_NAME_ARG file, FwAssertArgType arg1, FwAssertArgType arg2, FwSizeType lineNo) {
    return defaultSwAssert(file, lineNo, 2, arg1, arg2, 0, 0, 0, 0);
}

I8 SwAssert(FILE_NAME_ARG file, FwAssertArgType arg1, FwAssertArgType arg2, FwAssertArgType arg3, FwSizeType lineNo) {
    return defaultSwAssert(file, lineNo, 3, arg1, arg2, arg3, 0, 0, 0);
}

I8 SwAssert(FILE_NAME_ARG file,
            FwAssertArgType arg1,
            FwAssertArgType arg2,
            FwAssertArgType arg3,
            FwAssertArgType arg4,
            FwSizeType lineNo) {
    return defaultSwAssert(file, lineNo, 4, arg1, arg2, arg3, arg4, 0, 0);
}

I8 SwAssert(FILE_NAME_ARG file,
            FwAssertArgType arg1,
            FwAssertArgType arg2,
            FwAssertArgType arg3,
            FwAssertArgType arg4,
            FwAssertArgType arg5,
            FwSizeType lineNo) {
    return defaultSwAssert(file, lineNo, 5, arg1, arg2, arg3, arg4, arg5, 0);
}

I8 SwAssert(FILE_NAME_ARG file,
            FwAssertArgType arg1,
            FwAssertArgType arg2,
            FwAssertArgType arg3,
            FwAssertArgType arg4,
            FwAssertArgType arg5,
            FwAssertArgType arg6,
            FwSizeType lineNo) {
    return defaultSwAssert(file, lineNo, 6, arg1, arg2, arg3, arg4, arg5, arg6);
}
}  // namespace Fw

// define C asserts with C linkage
extern "C" {
I8 CAssert0(FILE_NAME_ARG file, FwSizeType lineNo);
I8 CAssert1(FILE_NAME_ARG file, FwAssertArgType arg1, FwSizeType lineNo);
}

I8 CAssert0(FILE_NAME_ARG file, FwSizeType lineNo) {
    if (nullptr == Fw::s_assertHook) {
        CHAR assertMsg[FW_ASSERT_TEXT_SIZE];
        Fw::defaultReportAssert(file, lineNo, 0, 0, 0, 0, 0, 0, 0, assertMsg,
                                static_cast<FwSizeType>(sizeof(assertMsg)));
    } else {
        Fw::s_assertHook->reportAssert(file, lineNo, 0, 0, 0, 0, 0, 0, 0);
        Fw::s_assertHook->doAssert();
    }
    return 0;
}

I8 CAssert1(FILE_NAME_ARG file, FwAssertArgType arg1, FwSizeType lineNo) {
    if (nullptr == Fw::s_assertHook) {
        CHAR assertMsg[FW_ASSERT_TEXT_SIZE];
        Fw::defaultReportAssert(file, lineNo, 1, arg1, 0, 0, 0, 0, 0, assertMsg,
                                static_cast<FwSizeType>(sizeof(assertMsg)));
    } else {
        Fw::s_assertHook->reportAssert(file, lineNo, 1, arg1, 0, 0, 0, 0, 0);
        Fw::s_assertHook->doAssert();
    }
    return 0;
}
