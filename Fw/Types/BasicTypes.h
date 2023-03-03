/**
 * \file: BasicType.h
 * \author mstarch
 * \brief Declares fprime basic types for usage within C language files
 *
 * \copyright
 * Copyright 2009-2016, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 *
 */
#include <PlatformTypes.h>
#ifndef FW_BASIC_TYPES_H
#define FW_BASIC_TYPES_H

// Compiler checks
#if defined(__GNUC__) || defined(__llvm__) || defined(PLATFORM_OVERRIDE_GCC_CLANG_CHECK)
#else
#error Unsupported compiler!
#endif

/*----------------------------------------------------------------------------*/
/* Type definitions:     I8, U8, I16, U16, ..., I64, U64, F32, and F64        */
/*----------------------------------------------------------------------------*/
typedef int8_t I8;   //!< 8-bit signed integer
typedef uint8_t U8;  //!< 8-bit unsigned integer
typedef U8 BYTE;     //!< byte type
typedef char CHAR;

#if FW_HAS_16_BIT
typedef int16_t I16;   //!< 16-bit signed integer
typedef uint16_t U16;  //!< 16-bit unsigned integer
#endif

#if FW_HAS_32_BIT
typedef uint32_t U32;  //!< 32-bit signed integer
typedef int32_t I32;   //!< 32-bit unsigned integer
#endif

#if FW_HAS_64_BIT
typedef int64_t I64;   //!< 64-bit signed integer
typedef uint64_t U64;  //!< 64-bit unsigned integer
#endif

typedef float F32;  //!< 32-bit floating point
#if FW_HAS_F64
typedef double F64;  //!< 64-bit floating point
#endif

// Backwards-compatibility definitions
typedef PlatformIntType NATIVE_INT_TYPE;
typedef PlatformUIntType NATIVE_UINT_TYPE;
typedef PlatformPointerCastType POINTER_CAST;

/*----------------------------------------------------------------------------*/
/* Useful macro definitions                                                   */
/*----------------------------------------------------------------------------*/
#define FW_NO_ASSERT 1  //!< Asserts turned off
#define FW_FILEID_ASSERT \
    2  //!< File ID used - requires -DASSERT_FILE_ID=somevalue to be set on the compile command line
#define FW_FILENAME_ASSERT 3  //!< Uses the file path in the assert - image stores filenames
#define FW_RELATIVE_PATH_ASSERT \
    4  //!< Uses a relative file path (within fprime/fprime library) for assert. - requires -DASSERT_RELATIVE_PATH=path
       //!< to be set on the compile command line

#define FW_NUM_ARRAY_ELEMENTS(a) (sizeof(a) / sizeof((a)[0]))  //!< number of elements in an array
#define FW_MAX(a, b) (((a) > (b)) ? (a) : (b))                 //!< MAX macro
#define FW_MIN(a, b) (((a) < (b)) ? (a) : (b))                 //!< MIN macro

#ifndef STATIC
#define STATIC static  //!< static for non unit-test code
#endif

#ifndef PROTECTED
#define PROTECTED protected  //!< overridable protected for unit testing
#endif

#ifndef PRIVATE
#define PRIVATE private  //!< overridable private for unit testing
#endif

#endif  // FW_BASIC_TYPES_H
