#include <FpConfig.hpp>
#include <Fw/Types/Assert.hpp>
#include <assert.h>
#include <stdio.h>

#ifdef TGT_OS_TYPE_VXWORKS
#include <taskLib.h>
#endif

#if FW_ASSERT_LEVEL == FW_NO_ASSERT

#else

#if FW_ASSERT_LEVEL == FW_FILEID_ASSERT
#define fileIdFs "Assert file ID 0x%08X: Line: %d "
#else
#define fileIdFs "Assert file \"%s\": Line: %d "
#endif

namespace Fw {

    void defaultPrintAssert(const I8* msg) {
        (void)fprintf(stderr,"%s\n",reinterpret_cast<const char*>(msg));
    }

    void defaultReportAssert
            (
            FILE_NAME_ARG file,
            NATIVE_UINT_TYPE lineNo,
            NATIVE_UINT_TYPE numArgs,
            AssertArg arg1,
            AssertArg arg2,
            AssertArg arg3,
            AssertArg arg4,
            AssertArg arg5,
            AssertArg arg6,
            I8* destBuffer,
            NATIVE_INT_TYPE buffSize
            ) {

        switch (numArgs) {
            case 0:
                (void)snprintf((char*)destBuffer,buffSize,fileIdFs,file,lineNo);
                break;
            case 1:
                (void)snprintf((char*)destBuffer,buffSize,fileIdFs "%d",file,lineNo,
                        arg1);
                break;
            case 2:
                (void)snprintf((char*)destBuffer,buffSize,fileIdFs "%d %d",file,lineNo,
                        arg1,arg2);
                break;
            case 3:
                (void)snprintf((char*)destBuffer,buffSize,fileIdFs "%d %d %d",file,lineNo,
                        arg1,arg2,arg3);
                break;
            case 4:
                (void)snprintf((char*)destBuffer,buffSize,fileIdFs "%d %d %d %d",file,lineNo,
                        arg1,arg2,arg3,arg4);
                break;
            case 5:
                (void)snprintf((char*)destBuffer,buffSize,fileIdFs "%d %d %d %d %d",file,lineNo,
                        arg1,arg2,arg3,arg4,arg5);
                break;
            case 6:
                (void)snprintf((char*)destBuffer,buffSize,fileIdFs "%d %d %d %d %d %d",file,lineNo,
                        arg1,arg2,arg3,arg4,arg5,arg6);
                break;
            default: // in an assert already, what can we do?
                break;
        }

        // null terminate
        destBuffer[buffSize-1] = 0;

    }

    void AssertHook::printAssert(const I8* msg) {
        defaultPrintAssert(msg);
    }

    void AssertHook::reportAssert
        (
            FILE_NAME_ARG file,
            NATIVE_UINT_TYPE lineNo,
            NATIVE_UINT_TYPE numArgs,
            AssertArg arg1,
            AssertArg arg2,
            AssertArg arg3,
            AssertArg arg4,
            AssertArg arg5,
            AssertArg arg6
         ) 
    {
        I8 destBuffer[256];
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

    void AssertHook::doAssert(void) {
        assert(0);
    }

    STATIC AssertHook* s_assertHook = NULL;

    void AssertHook::registerHook(void) {
        this->previousHook = s_assertHook;
        s_assertHook = this;
    }

    void AssertHook::deregisterHook() {
        s_assertHook = this->previousHook;
    }

    NATIVE_INT_TYPE SwAssert(FILE_NAME_ARG file, NATIVE_UINT_TYPE lineNo) {
        if (NULL == s_assertHook) {
            I8 assertMsg[256];
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
            AssertArg arg1) {
        if (NULL == s_assertHook) {
            I8 assertMsg[256];
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
            AssertArg arg1,
            AssertArg arg2) {
        if (NULL == s_assertHook) {
            I8 assertMsg[256];
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
            AssertArg arg1,
            AssertArg arg2,
            AssertArg arg3) {
        if (NULL == s_assertHook) {
            I8 assertMsg[256];
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
            AssertArg arg1,
            AssertArg arg2,
            AssertArg arg3,
            AssertArg arg4) {
        if (NULL == s_assertHook) {
            I8 assertMsg[256];
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
            AssertArg arg1,
            AssertArg arg2,
            AssertArg arg3,
            AssertArg arg4,
            AssertArg arg5) {
        if (NULL == s_assertHook) {
            I8 assertMsg[256];
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
            AssertArg arg1,
            AssertArg arg2,
            AssertArg arg3,
            AssertArg arg4,
            AssertArg arg5,
            AssertArg arg6) {
        if (NULL == s_assertHook) {
            I8 assertMsg[256];
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
    if (NULL == Fw::s_assertHook) {
        I8 assertMsg[256];
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

