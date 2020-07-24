/*
 * UnitTestAssert.cpp
 *
 *  Created on: Feb 8, 2016
 *      Author: tcanham
 *  Revised July 2020
 *      Author: bocchino
 */

#include <Fw/Test/UnitTestAssert.hpp>
#include <stdio.h>
#include <string.h>

namespace Test {

    UnitTestAssert::UnitTestAssert() :
      m_lineNo(0),
      m_numArgs(0),
      m_arg1(0),
      m_arg2(0),
      m_arg3(0),
      m_arg4(0),
      m_arg5(0),
      m_arg6(0),
      m_assertFailed(false)
    {

#if FW_ASSERT_LEVEL == FW_FILEID_ASSERT
      this->m_file = 0;
#else
      memset(this->m_file, 0, sizeof(this->m_file));
#endif

        // save the current hook
        this->m_previousAssertHook = Fw::AssertHook::getAssertHook();
        // register this hook
        Fw::AssertHook::registerHook();
    }

    UnitTestAssert::~UnitTestAssert() {
        // restore previous hook
        Fw::AssertHook::setAssertHook(this->m_previousAssertHook);
    }

    void UnitTestAssert::doAssert(void) {
        this->m_assertFailed = true;
#if FW_ASSERT_LEVEL == FW_FILEID_ASSERT
        (void)fprintf(stderr,"Assert File: 0x%x, Line: %u\n", this->m_file, this->m_lineNo);
#else
        (void)fprintf(stderr,"Assert File: %s, Line: %u\n", this->m_file, this->m_lineNo);
#endif
    }

    void UnitTestAssert::reportAssert(
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

#if FW_ASSERT_LEVEL == FW_FILEID_ASSERT
        this->m_file = file;
#else
        strncpy(
            reinterpret_cast<char*>(this->m_file),
            reinterpret_cast<const char*>(file),
            sizeof(this->m_file)
        );
#endif
        this->m_lineNo = lineNo;
        this->m_numArgs = numArgs;
        this->m_arg1 = arg1;
        this->m_arg2 = arg2;
        this->m_arg3 = arg3;
        this->m_arg4 = arg4;
        this->m_arg5 = arg5;
        this->m_arg6 = arg6;


    }

    void UnitTestAssert::retrieveAssert(
                    FILE_NAME_ARG file,
                    NATIVE_UINT_TYPE& lineNo,
                    NATIVE_UINT_TYPE& numArgs,
                    AssertArg& arg1,
                    AssertArg& arg2,
                    AssertArg& arg3,
                    AssertArg& arg4,
                    AssertArg& arg5,
                    AssertArg& arg6
                    ) const {

#if FW_ASSERT_LEVEL == FW_FILEID_ASSERT
        file = this->m_file;
#else
        strncpy(
            reinterpret_cast<char*>(file),
            reinterpret_cast<const char*>(this->m_file),
            sizeof(this->m_file)
        );
#endif
        lineNo = this->m_lineNo;
        numArgs = this->m_numArgs;
        arg1 = this->m_arg1;
        arg2 = this->m_arg2;
        arg3 = this->m_arg3;
        arg4 = this->m_arg4;
        arg5 = this->m_arg5;
        arg6 = this->m_arg6;
    }

    bool UnitTestAssert::assertFailed() const {
        return this->m_assertFailed;
    }

    void UnitTestAssert::clearAssertFailure() {
        this->m_assertFailed = false;
    }

} /* namespace Test */
