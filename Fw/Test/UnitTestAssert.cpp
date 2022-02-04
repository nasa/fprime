/*
 * UnitTestAssert.cpp
 *
 *  Created on: Feb 8, 2016
 *      Author: tcanham
 *  Revised July 2020
 *      Author: bocchino
 */

#include <Fw/Test/UnitTestAssert.hpp>
#include <cstdio>
#include <cstring>

namespace Test {

#if FW_ASSERT_LEVEL == FW_FILEID_ASSERT
    const UnitTestAssert::File UnitTestAssert::fileInit = 0;
#else
    const UnitTestAssert::File UnitTestAssert::fileInit = "";
#endif

    UnitTestAssert::UnitTestAssert() :
      m_file(fileInit),
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
        // register this hook
        Fw::AssertHook::registerHook();
    }

    UnitTestAssert::~UnitTestAssert() {
        // deregister the hook
        Fw::AssertHook::deregisterHook();
    }

    void UnitTestAssert::doAssert() {
        this->m_assertFailed = true;
#if FW_ASSERT_LEVEL == FW_FILEID_ASSERT
        (void)fprintf(stderr,"Assert File: 0x%x, Line: %u\n", this->m_file, this->m_lineNo);
#else
        (void)fprintf(stderr,"Assert File: %s, Line: %u\n", this->m_file.toChar(), this->m_lineNo);
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
        this->m_file = reinterpret_cast<const char*>(file);
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
                    File& file,
                    NATIVE_UINT_TYPE& lineNo,
                    NATIVE_UINT_TYPE& numArgs,
                    AssertArg& arg1,
                    AssertArg& arg2,
                    AssertArg& arg3,
                    AssertArg& arg4,
                    AssertArg& arg5,
                    AssertArg& arg6
                    ) const {

        file = this->m_file;
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
