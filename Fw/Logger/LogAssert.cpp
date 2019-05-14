/*
 * LogAssert.cpp
 *
 *  Created on: Sep 9, 2016
 *      Author: tcanham
 * Note: this file was originally a  log assert file, under Fw::Types. It now made generic
 * to log asserts to Fw::Logger
 */

#include <Fw/Logger/LogAssert.hpp>
#include <Fw/Logger/Logger.hpp>

#if FW_ASSERT_LEVEL == FW_NO_ASSERT

#else

#if FW_ASSERT_LEVEL == FW_FILEID_ASSERT
#define fileIdFs "Assert file ID %d: Line: %d "
#else
#define fileIdFs "Assert file \"%s\": Line: %d "
#endif


namespace Fw {

    LogAssertHook::LogAssertHook() {

    }

    LogAssertHook::~LogAssertHook() {
    }

    void LogAssertHook::reportAssert(
        FILE_NAME_ARG file,
        NATIVE_UINT_TYPE lineNo,
        NATIVE_UINT_TYPE numArgs,
        AssertArg arg1,
        AssertArg arg2,
        AssertArg arg3,
        AssertArg arg4,
        AssertArg arg5,
        AssertArg arg6
    ) {
        // Assumption is that file (when string) goes back to static macro in the code and will persist
        switch (numArgs) {
            case 0:
                Fw::Logger::logMsg(fileIdFs,reinterpret_cast<POINTER_CAST>(file),lineNo,0,0,0,0);
                break;
            case 1:
                Fw::Logger::logMsg(fileIdFs " %d\n",reinterpret_cast<POINTER_CAST>(file),lineNo,arg1,0,0,0);
                break;
            case 2:
                Fw::Logger::logMsg(fileIdFs " %d %d\n",reinterpret_cast<POINTER_CAST>(file),lineNo,arg1,arg2,0,0);
                break;
            case 3:
                Fw::Logger::logMsg(fileIdFs " %d %d %d\n",reinterpret_cast<POINTER_CAST>(file),lineNo,arg1,arg2,arg3,0);
                break;
            case 4:
                Fw::Logger::logMsg(fileIdFs " %d %d %d %d\n",reinterpret_cast<POINTER_CAST>(file),lineNo,arg1,arg2,arg3,arg4);
                break;
            default: // can't fit remainder of arguments in log message
                Fw::Logger::logMsg(fileIdFs " %d %d %d %d +\n",reinterpret_cast<POINTER_CAST>(file),lineNo,arg1,arg2,arg3,arg4);
                break;
        }

    }

    void LogAssertHook::printAssert(const I8* msg) {
        // do nothing since reportAssert() sends message
    }

    void LogAssertHook::doAssert(void) {
    }

} // namespace Fw

#endif
