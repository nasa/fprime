/*
 * UnitTestAssert.hpp
 *
 *  Created on: Feb 8, 2016
 *      Author: tcanham
 */

#ifndef TEST_UNITTESTASSERT_HPP_
#define TEST_UNITTESTASSERT_HPP_

#include <Fw/Types/Assert.hpp>

namespace Test {

    class UnitTestAssert: public Fw::AssertHook {
        public:
            UnitTestAssert();
            virtual ~UnitTestAssert();
            // function for hook
            void doAssert(void);
            void reportAssert(
                    FILE_NAME_ARG file,
                    NATIVE_UINT_TYPE lineNo,
                    NATIVE_UINT_TYPE numArgs,
                    AssertArg arg1,
                    AssertArg arg2,
                    AssertArg arg3,
                    AssertArg arg4,
                    AssertArg arg5,
                    AssertArg arg6
                    );
            // function to retrieve assert values
            void retrieveAssert(
                FILE_NAME_ARG& file,
                NATIVE_UINT_TYPE& lineNo,
                NATIVE_UINT_TYPE& numArgs,
                AssertArg& arg1,
                AssertArg& arg2,
                AssertArg& arg3,
                AssertArg& arg4,
                AssertArg& arg5,
                AssertArg& arg6
                );

        private:
#if FW_ASSERT_LEVEL == FW_FILEID_ASSERT
            FILE_NAME_ARG m_file;
#else
            FILE_NAME_ARG m_file[256];
#endif
            NATIVE_UINT_TYPE m_lineNo;
            NATIVE_INT_TYPE m_numArgs;
            AssertArg m_arg1;
            AssertArg m_arg2;
            AssertArg m_arg3;
            AssertArg m_arg4;
            AssertArg m_arg5;
            AssertArg m_arg6;

    };

} /* namespace Test */

#endif /* TEST_UNITTESTASSERT_HPP_ */
