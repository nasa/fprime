#include <FpConfig.hpp>
#include <Fw/Types/Assert.hpp>
#include <cassert>
#include <cstdio>

#define FW_ASSERT_DFL_MSG_LEN 256

#if FW_ASSERT_LEVEL == FW_NO_ASSERT

#else

#if FW_ASSERT_LEVEL == FW_FILEID_ASSERT
#define fileIdFs "Assert file ID 0x%08" PRIx32 ": Line: %" PRI_PlatformUIntType
#else
#define fileIdFs "Assert file \"%s\": Line: %" PRI_PlatformUIntType
#endif

namespace Fw {

    void defaultPrintAssert(const CHAR* msg) {
        (void)fprintf(stderr,"%s\n", msg);
    }

    void defaultReportAssert
            (
            FILE_NAME_ARG file,
            NATIVE_UINT_TYPE lineNo,
            NATIVE_UINT_TYPE numArgs,
            FwAssertArgType arg1,
            FwAssertArgType arg2,
            FwAssertArgType arg3,
            FwAssertArgType arg4,
            FwAssertArgType arg5,
            FwAssertArgType arg6,
            CHAR* destBuffer,
            NATIVE_INT_TYPE buffSize
            ) {

        switch (numArgs) {
            case 0:
                (void) snprintf(destBuffer, buffSize, fileIdFs, file, lineNo);
                break;
            case 1:
                (void) snprintf(
                    destBuffer,
                    buffSize,
                    fileIdFs " %" PRI_FwAssertArgType,
                    file,
                    lineNo,
                    arg1
                );
                break;
            case 2:
                (void) snprintf(
                    destBuffer,
                    buffSize,
                    fileIdFs " %" PRI_FwAssertArgType " %" PRI_FwAssertArgType,
                    file,
                    lineNo,
                    arg1, arg2
                );
                break;
            case 3:
                (void) snprintf(
                    destBuffer,
                    buffSize,
                    fileIdFs " %" PRI_FwAssertArgType " %" PRI_FwAssertArgType
                      " %" PRI_FwAssertArgType,
                    file,
                    lineNo,
                    arg1, arg2, arg3
                );
                break;
            case 4:
                (void) snprintf(
                    destBuffer,
                    buffSize,
                    fileIdFs " %" PRI_FwAssertArgType " %" PRI_FwAssertArgType
                      " %" PRI_FwAssertArgType " %" PRI_FwAssertArgType,
                    file,
                    lineNo,
                    arg1, arg2, arg3, arg4);
                break;
            case 5:
                (void) snprintf(
                    destBuffer,
                    buffSize,
                    fileIdFs " %" PRI_FwAssertArgType " %" PRI_FwAssertArgType
                      " %" PRI_FwAssertArgType " %" PRI_FwAssertArgType
                      " %" PRI_FwAssertArgType,
                    file,
                    lineNo,
                    arg1, arg2, arg3, arg4, arg5
                );
                break;
            case 6:
                (void) snprintf(
                    destBuffer,
                    buffSize,
                    fileIdFs " %" PRI_FwAssertArgType " %" PRI_FwAssertArgType
                      " %" PRI_FwAssertArgType " %" PRI_FwAssertArgType
                      " %" PRI_FwAssertArgType " %" PRI_FwAssertArgType,
                    file,
                    lineNo,
                    arg1, arg2, arg3, arg4, arg5, arg6
                );
                break;
            default: // in an assert already, what can we do?
                break;
        }

        // null terminate
        destBuffer[buffSize-1] = 0;

    }

    void AssertHook::printAssert(const CHAR* msg) {
        defaultPrintAssert(msg);
    }

    void AssertHook::reportAssert
        (
            FILE_NAME_ARG file,
            NATIVE_UINT_TYPE lineNo,
            NATIVE_UINT_TYPE numArgs,
            FwAssertArgType arg1,
            FwAssertArgType arg2,
            FwAssertArgType arg3,
            FwAssertArgType arg4,
            FwAssertArgType arg5,
            FwAssertArgType arg6
         )
    {
        CHAR destBuffer[FW_ASSERT_DFL_MSG_LEN];
        defaultReportAssert
        (
            file,
            lineNo,
            numArgs,
            arg1,
            arg2,
            arg3,
            arg4,
            arg5,
            arg6,
            destBuffer,
            sizeof(destBuffer)
         );
        // print message
        this->printAssert(destBuffer);
    }

    void AssertHook::doAssert() {
        assert(0);
    }

    STATIC AssertHook* s_assertHook = nullptr;

    void AssertHook::registerHook() {
        this->previousHook = s_assertHook;
        s_assertHook = this;
    }

    void AssertHook::deregisterHook() {
        s_assertHook = this->previousHook;
    }

    NATIVE_INT_TYPE SwAssert(FILE_NAME_ARG file, NATIVE_UINT_TYPE lineNo) {
        if (nullptr == s_assertHook) {
            CHAR assertMsg[FW_ASSERT_DFL_MSG_LEN];
            defaultReportAssert(
                file,
                lineNo,
                0,
                0,0,0,0,0,0,
                assertMsg,sizeof(assertMsg));
                // print message
            defaultPrintAssert(assertMsg);
            assert(0);
        }
        else {
            s_assertHook->reportAssert(
                file,
                lineNo,
                0,
                0,0,0,0,0,0);
            s_assertHook->doAssert();
        }
        return 0;
    }

    NATIVE_INT_TYPE SwAssert(FILE_NAME_ARG file, NATIVE_UINT_TYPE lineNo,
            FwAssertArgType arg1) {
        if (nullptr == s_assertHook) {
            CHAR assertMsg[FW_ASSERT_DFL_MSG_LEN];
            defaultReportAssert(
                file,
                lineNo,
                1,
                arg1,0,0,0,0,0,
                assertMsg,sizeof(assertMsg));
            // print message
            defaultPrintAssert(assertMsg);
            assert(0);
        }
        else {
            s_assertHook->reportAssert(
                file,
                lineNo,
                1,
                arg1,0,0,0,0,0);
            s_assertHook->doAssert();
        }
        return 0;
    }

    NATIVE_INT_TYPE SwAssert(FILE_NAME_ARG file, NATIVE_UINT_TYPE lineNo,
            FwAssertArgType arg1,
            FwAssertArgType arg2) {
        if (nullptr == s_assertHook) {
            CHAR assertMsg[FW_ASSERT_DFL_MSG_LEN];
            defaultReportAssert(
                file,
                lineNo,
                2,
                arg1,arg2,0,0,0,0,
                assertMsg,sizeof(assertMsg));
            defaultPrintAssert(assertMsg);
            assert(0);
        }
        else {
            s_assertHook->reportAssert(
                file,
                lineNo,
                2,
                arg1,arg2,0,0,0,0);
            s_assertHook->doAssert();
        }
        return 0;
    }

    NATIVE_INT_TYPE SwAssert(FILE_NAME_ARG file, NATIVE_UINT_TYPE lineNo,
            FwAssertArgType arg1,
            FwAssertArgType arg2,
            FwAssertArgType arg3) {
        if (nullptr == s_assertHook) {
            CHAR assertMsg[FW_ASSERT_DFL_MSG_LEN];
            defaultReportAssert(
                file,
                lineNo,
                3,
                arg1,arg2,arg3,0,0,0,
                assertMsg,sizeof(assertMsg));
            defaultPrintAssert(assertMsg);
            assert(0);
        }
        else {
            s_assertHook->reportAssert(
                file,
                lineNo,
                3,
                arg1,arg2,arg3,0,0,0);
            s_assertHook->doAssert();
        }
        return 0;
    }

    NATIVE_INT_TYPE SwAssert(FILE_NAME_ARG file, NATIVE_UINT_TYPE lineNo,
            FwAssertArgType arg1,
            FwAssertArgType arg2,
            FwAssertArgType arg3,
            FwAssertArgType arg4) {
        if (nullptr == s_assertHook) {
            CHAR assertMsg[FW_ASSERT_DFL_MSG_LEN];
            defaultReportAssert(
                file,
                lineNo,
                4,
                arg1,arg2,arg3,arg4,0,0,
                assertMsg,sizeof(assertMsg));
            defaultPrintAssert(assertMsg);
            assert(0);
        }
        else {
            s_assertHook->reportAssert(
                file,
                lineNo,
                4,
                arg1,arg2,arg3,arg4,0,0);
            s_assertHook->doAssert();
        }
        return 0;
    }

    NATIVE_INT_TYPE SwAssert(FILE_NAME_ARG file, NATIVE_UINT_TYPE lineNo,
            FwAssertArgType arg1,
            FwAssertArgType arg2,
            FwAssertArgType arg3,
            FwAssertArgType arg4,
            FwAssertArgType arg5) {
        if (nullptr == s_assertHook) {
            CHAR assertMsg[FW_ASSERT_DFL_MSG_LEN];
            defaultReportAssert(
                file,
                lineNo,
                5,
                arg1,arg2,arg3,arg4,arg5,0,
                assertMsg,sizeof(assertMsg));
            defaultPrintAssert(assertMsg);
            assert(0);
        }
        else {
            s_assertHook->reportAssert(
                file,
                lineNo,
                5,
                arg1,arg2,arg3,arg4,arg5,0);
            s_assertHook->doAssert();
        }
        return 0;
    }

    NATIVE_INT_TYPE SwAssert(FILE_NAME_ARG file, NATIVE_UINT_TYPE lineNo,
            FwAssertArgType arg1,
            FwAssertArgType arg2,
            FwAssertArgType arg3,
            FwAssertArgType arg4,
            FwAssertArgType arg5,
            FwAssertArgType arg6) {
        if (nullptr == s_assertHook) {
            CHAR assertMsg[FW_ASSERT_DFL_MSG_LEN];
            defaultReportAssert(
                file,
                lineNo,
                6,
                arg1,arg2,arg3,arg4,arg5,arg6,
                assertMsg,sizeof(assertMsg));
            defaultPrintAssert(assertMsg);
            assert(0);
        }
        else {
            s_assertHook->reportAssert(
                file,
                lineNo,
                6,
                arg1,arg2,arg3,arg4,arg5,arg6);
            s_assertHook->doAssert();
        }
        return 0;
    }
}

// define C asserts.
extern "C" {
    NATIVE_INT_TYPE CAssert0(FILE_NAME_ARG file, NATIVE_UINT_TYPE lineNo);
}

NATIVE_INT_TYPE CAssert0(FILE_NAME_ARG file, NATIVE_UINT_TYPE lineNo) {
    if (nullptr == Fw::s_assertHook) {
        CHAR assertMsg[FW_ASSERT_DFL_MSG_LEN];
        Fw::defaultReportAssert(
            file,
            lineNo,
            0,
            0,0,0,0,0,0,
            assertMsg,sizeof(assertMsg));
    }
    else {
        Fw::s_assertHook->reportAssert(
            file,
            lineNo,
            0,
            0,0,0,0,0,0);
        Fw::s_assertHook->doAssert();
    }
    return 0;
}

#endif // FW_NO_ASSERT

