// ======================================================================
// \title  fputs_assert_hook.cpp
// \author Andrei Tumbar
// \brief  cpp file for assert hook functions using fputs implementation
// ======================================================================
#include <Fw/Types/assert_hook.hpp>
#include <Fw/Types/format.hpp>
#include <cstdio>

#if FW_ASSERT_LEVEL == FW_FILEID_ASSERT
#define fileIdFs "Assert: 0x%08" PRIx32 ":%" PRI_FwSizeType ""
#else
#define fileIdFs "Assert: \"%s:%" PRI_FwSizeType "\""
#endif

void Fw::defaultPrintAssert(const CHAR* msg) {
    // Write to stderr w/o formatting
    (void)fputs(msg, stderr);
    (void)fputs("\n", stderr);
}

void Fw::defaultReportAssert(FILE_NAME_ARG file,
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
