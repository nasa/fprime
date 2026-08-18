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
#define FW_CASSERT_2(cond, arg1, arg2)
#define FW_CASSERT_3(cond, arg1, arg2, arg3)
#define FW_CASSERT_4(cond, arg1, arg2, arg3, arg4)
#define FW_CASSERT_5(cond, arg1, arg2, arg3, arg4, arg5)
#define FW_CASSERT_6(cond, arg1, arg2, arg3, arg4, arg5, arg6)

#else  // ASSERT is defined

#if FW_ASSERT_LEVEL == FW_FILEID_ASSERT
#define FILE_NAME_ARG U32

#define FW_CASSERT(cond) ((void)((cond) ? (0) : (CAssert0(ASSERT_FILE_ID, __LINE__))))

#define FW_CASSERT_1(cond, arg1) ((void)((cond) ? (0) : (CAssert1(ASSERT_FILE_ID, (FwAssertArgType)(arg1), __LINE__))))

#define FW_CASSERT_2(cond, arg1, arg2) \
    ((void)((cond) ? (0) : (CAssert2(ASSERT_FILE_ID, (FwAssertArgType)(arg1), (FwAssertArgType)(arg2), __LINE__))))

#define FW_CASSERT_3(cond, arg1, arg2, arg3)                                                     \
    ((void)((cond) ? (0)                                                                         \
                   : (CAssert3(ASSERT_FILE_ID, (FwAssertArgType)(arg1), (FwAssertArgType)(arg2), \
                               (FwAssertArgType)(arg3), __LINE__))))

#define FW_CASSERT_4(cond, arg1, arg2, arg3, arg4)                                               \
    ((void)((cond) ? (0)                                                                         \
                   : (CAssert4(ASSERT_FILE_ID, (FwAssertArgType)(arg1), (FwAssertArgType)(arg2), \
                               (FwAssertArgType)(arg3), (FwAssertArgType)(arg4), __LINE__))))

#define FW_CASSERT_5(cond, arg1, arg2, arg3, arg4, arg5)                                         \
    ((void)((cond) ? (0)                                                                         \
                   : (CAssert5(ASSERT_FILE_ID, (FwAssertArgType)(arg1), (FwAssertArgType)(arg2), \
                               (FwAssertArgType)(arg3), (FwAssertArgType)(arg4), (FwAssertArgType)(arg5), __LINE__))))

#define FW_CASSERT_6(cond, arg1, arg2, arg3, arg4, arg5, arg6)                                                         \
    ((void)((cond)                                                                                                     \
                ? (0)                                                                                                  \
                : (CAssert6(ASSERT_FILE_ID, (FwAssertArgType)(arg1), (FwAssertArgType)(arg2), (FwAssertArgType)(arg3), \
                            (FwAssertArgType)(arg4), (FwAssertArgType)(arg5), (FwAssertArgType)(arg6), __LINE__))))

#else  // FW_ASSERT_LEVEL != FW_FILEID_ASSERT
#define FILE_NAME_ARG const CHAR*

#define FW_CASSERT(cond) ((void)((cond) ? (0) : (CAssert0((FILE_NAME_ARG)(__FILE__), __LINE__))))

#define FW_CASSERT_1(cond, arg1) \
    ((void)((cond) ? (0) : (CAssert1((FILE_NAME_ARG)(__FILE__), (FwAssertArgType)(arg1), __LINE__))))

#define FW_CASSERT_2(cond, arg1, arg2) \
    ((void)((cond)                     \
                ? (0)                  \
                : (CAssert2((FILE_NAME_ARG)(__FILE__), (FwAssertArgType)(arg1), (FwAssertArgType)(arg2), __LINE__))))

#define FW_CASSERT_3(cond, arg1, arg2, arg3)                                                                \
    ((void)((cond) ? (0)                                                                                    \
                   : (CAssert3((FILE_NAME_ARG)(__FILE__), (FwAssertArgType)(arg1), (FwAssertArgType)(arg2), \
                               (FwAssertArgType)(arg3), __LINE__))))

#define FW_CASSERT_4(cond, arg1, arg2, arg3, arg4)                                                          \
    ((void)((cond) ? (0)                                                                                    \
                   : (CAssert4((FILE_NAME_ARG)(__FILE__), (FwAssertArgType)(arg1), (FwAssertArgType)(arg2), \
                               (FwAssertArgType)(arg3), (FwAssertArgType)(arg4), __LINE__))))

#define FW_CASSERT_5(cond, arg1, arg2, arg3, arg4, arg5)                                                    \
    ((void)((cond) ? (0)                                                                                    \
                   : (CAssert5((FILE_NAME_ARG)(__FILE__), (FwAssertArgType)(arg1), (FwAssertArgType)(arg2), \
                               (FwAssertArgType)(arg3), (FwAssertArgType)(arg4), (FwAssertArgType)(arg5), __LINE__))))

#define FW_CASSERT_6(cond, arg1, arg2, arg3, arg4, arg5, arg6)                                              \
    ((void)((cond) ? (0)                                                                                    \
                   : (CAssert6((FILE_NAME_ARG)(__FILE__), (FwAssertArgType)(arg1), (FwAssertArgType)(arg2), \
                               (FwAssertArgType)(arg3), (FwAssertArgType)(arg4), (FwAssertArgType)(arg5),   \
                               (FwAssertArgType)(arg6), __LINE__))))

#endif  // FW_ASSERT_LEVEL == FW_FILEID_ASSERT

I8 CAssert0(FILE_NAME_ARG file, FwSizeType lineNo);                        //!< C assert function
I8 CAssert1(FILE_NAME_ARG file, FwAssertArgType arg1, FwSizeType lineNo);  //!< C assert function with one argument
//! C assert function with two arguments
I8 CAssert2(FILE_NAME_ARG file, FwAssertArgType arg1, FwAssertArgType arg2, FwSizeType lineNo);
//! C assert function with three arguments
I8 CAssert3(FILE_NAME_ARG file, FwAssertArgType arg1, FwAssertArgType arg2, FwAssertArgType arg3, FwSizeType lineNo);
//! C assert function with four arguments
I8 CAssert4(FILE_NAME_ARG file,
            FwAssertArgType arg1,
            FwAssertArgType arg2,
            FwAssertArgType arg3,
            FwAssertArgType arg4,
            FwSizeType lineNo);
//! C assert function with five arguments
I8 CAssert5(FILE_NAME_ARG file,
            FwAssertArgType arg1,
            FwAssertArgType arg2,
            FwAssertArgType arg3,
            FwAssertArgType arg4,
            FwAssertArgType arg5,
            FwSizeType lineNo);
//! C assert function with six arguments
I8 CAssert6(FILE_NAME_ARG file,
            FwAssertArgType arg1,
            FwAssertArgType arg2,
            FwAssertArgType arg3,
            FwAssertArgType arg4,
            FwAssertArgType arg5,
            FwAssertArgType arg6,
            FwSizeType lineNo);

#endif  // ASSERT is defined

#ifdef __cplusplus
}
#endif

#endif /* FWCASSERT_HPP_ */
