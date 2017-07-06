/*
 * UnitTestAssert.cpp
 *
 *  Created on: Feb 8, 2016
 *      Author: tcanham
 */

#include <Fw/Test/UnitTestAssert.hpp>
#include <stdio.h>
#include <string.h>

namespace Test {

    UnitTestAssert::UnitTestAssert() {

        // register this hook
        Fw::AssertHook::registerHook();
    }

    UnitTestAssert::~UnitTestAssert() {
        // deregister hook
        Fw::AssertHook::registerHook();
    }

    void UnitTestAssert::doAssert(void) {
        (void)fprintf(stderr,"Assert File: %d, Line: %d\n", this->m_file, this->m_lineNo);
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

        //printf("Test Assert Called!\n");
#if FW_ASSERT_LEVEL == FW_FILEID_ASSERT
        this->m_file = file;
#else
        memcpy(this->m_file,file,sizeof(this->m_file));
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
                    FILE_NAME_ARG& file,
                    NATIVE_UINT_TYPE& lineNo,
                    NATIVE_UINT_TYPE& numArgs,
                    AssertArg& arg1,
                    AssertArg& arg2,
                    AssertArg& arg3,
                    AssertArg& arg4,
                    AssertArg& arg5,
                    AssertArg& arg6
                    ) {

        file = (U8*)this->m_file;
        lineNo = this->m_lineNo;
        numArgs = this->m_numArgs;
        arg1 = this->m_arg1;
        arg2 = this->m_arg2;
        arg3 = this->m_arg3;
        arg4 = this->m_arg4;
        arg5 = this->m_arg5;
        arg6 = this->m_arg6;
    }

} /* namespace Test */
