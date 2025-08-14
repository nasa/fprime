/*
 * FwCAssert.hpp
 *
 *  Created on: Jun 8, 2014
 *      Author: tcanham
 */

#ifndef FWCASSERT_HPP_
#define FWCASSERT_HPP_

#include <Fw/FPrimeBasicTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#if FW_ASSERT_LEVEL == FW_NO_ASSERT

#define FW_CASSERT(...)
#define FW_CASSERT_1(cond, arg1)

#else  // ASSERT is defined

#if FW_ASSERT_LEVEL == FW_FILEID_ASSERT
#define FILE_NAME_ARG U32
#define FW_CASSERT(cond) ((void)((cond) ? (0) : (CAssert0(ASSERT_FILE_ID, __LINE__))))
#define FW_CASSERT_1(cond, arg1) ((void)((cond) ? (0) : (CAssert1(ASSERT_FILE_ID, (FwAssertArgType)(arg1), __LINE__))))
#else
#define FILE_NAME_ARG const CHAR*
#define FW_CASSERT(cond) ((void)((cond) ? (0) : (CAssert0((FILE_NAME_ARG)(__FILE__), __LINE__))))
#define FW_CASSERT_1(cond, arg1) \
    ((void)((cond) ? (0) : (CAssert1((FILE_NAME_ARG)(__FILE__), (FwAssertArgType)(arg1), __LINE__))))
#endif

I8 CAssert0(FILE_NAME_ARG file, FwSizeType lineNo);                        //!< C assert function
I8 CAssert1(FILE_NAME_ARG file, FwAssertArgType arg1, FwSizeType lineNo);  //!< C assert function with one argument

#endif  // ASSERT is defined

#ifdef __cplusplus
}
#endif

#endif /* FWCASSERT_HPP_ */
