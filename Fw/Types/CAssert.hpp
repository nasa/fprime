/*
 * FwCAssert.hpp
 *
 *  Created on: Jun 8, 2014
 *      Author: tcanham
 */

# FWCASSERT_HPP_
# FWCASSERT_HPP_

# <FpConfig.hpp>
# <Fw/Types/BasicTypes.hpp>

# FW_ASSERT_LEVEL == FW_NO_ASSERT

# FW_CASSERT(...)

# // ASSERT is defined

# FW_ASSERT_LEVEL == FW_FILEID_ASSERT
# FILE_NAME_ARG NATIVE_UINT_TYPE
# FW_CASSERT() \
    (() (() ? (0) : \
    (CAssert0(ASSERT_FILE_ID, __LINE__))))
#
# FILE_NAME_ARG  U8*
# FW_CASSERT() \
    (() (() ? (0) : \
    (CAssert0((U8*)__FILE__, __LINE__))))
#

# __cplusplus
 "C" {
# // __cplusplus

I32 CAssert0(FILE_NAME_ARG file, U32 lineNo); //!< C assert function
I32 CAssert1(FILE_NAME_ARG file, U32 lineNo, NATIVE_INT_TYPE arg1); //!< C assert function 1

# __cplusplus
} // extern "C"
# // __cplusplus



#endif // ASSERT is defined
#endif /* FWCASSERT_HPP_ */
