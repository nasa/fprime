/*
 * FwCAssert.hpp
 *
 *  Created on: Jun 8, 2014
 *      Author: tcanham
 */

#ifndef FWCASSERT_HPP_
#define FWCASSERT_HPP_

#include <FpConfig.h>

#ifdef  __cplusplus
extern "C" {
#endif

#if FW_ASSERT_LEVEL == FW_NO_ASSERT

#define FW_CASSERT(...)

#else  // ASSERT is defined

#if FW_ASSERT_LEVEL == FW_FILEID_ASSERT
#define FILE_NAME_ARG NATIVE_UINT_TYPE
#define FW_CASSERT(cond) ((void)((cond) ? (0) : (CAssert0(ASSERT_FILE_ID, __LINE__))))
#else
#define FILE_NAME_ARG const CHAR*
#define FW_CASSERT(cond) ((void)((cond) ? (0) : (CAssert0((FILE_NAME_ARG)(__FILE__), __LINE__))))
#endif

I32 CAssert0(FILE_NAME_ARG file, U32 lineNo);                        //!< C assert function
I32 CAssert1(FILE_NAME_ARG file, U32 lineNo, NATIVE_INT_TYPE arg1);  //!< C assert function 1

#endif  // ASSERT is defined

#ifdef  __cplusplus
}
#endif

#endif  /* FWCASSERT_HPP_ */
