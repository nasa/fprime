// ======================================================================
// \title  Fw/Types/BasicTypes.h
// \author mstarch
// \brief  h file for FPrime basic numerical aliases (I8, U64, etc.)
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// FPrime allows use of shorthand fixed-with types. This file provides
// these definitions. I# is a signed integer of width #, U# is an unsigned
// integer of width #, F# is a floating point number of width #.
//
// This file also contains macros for a number of useful operations:
//
// - FW_NUM_ARRAY_ELEMENTS(a): number of elements in an array
// - FW_MAX(a, b): maximum of a and b
// - FW_MIN(a, b): minimum of a and b
//
// - FW_NO_ASSERT: constant for assertions turned off
// - FW_FILEID_ASSERT: constant for assertions reported as a file CRC and line number
// - FW_FILENAME_ASSERT: constant for assertions reported as a file path and line number
// - FW_RELATIVE_PATH_ASSERT: constant for assertions reported as a relative path within
//       FPrime and line number
//
//
// This header is intended to be C-compatible.
//
// ======================================================================
#ifndef FW_BASIC_TYPES_H
#define FW_BASIC_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif
#include <config/FPrimeNumericalConfig.h>
#include <inttypes.h>  // Standard integer types and printf macros

// Compiler checks
#if defined(__GNUC__) || defined(__llvm__) || defined(PLATFORM_OVERRIDE_GCC_CLANG_CHECK)
#else
#error \
    "FPrime only supports GCC or Clang compilers. You may attempt to use other compilers by defining PLATFORM_OVERRIDE_GCC_CLANG_CHECK, but this is not recommended."
#endif

/*----------------------------------------------------------------------------*/
/* Type definitions:     I8, U8, I16, U16, ..., I64, U64, F32, and F64        */
/*----------------------------------------------------------------------------*/
typedef int8_t I8;  //!< 8-bit signed integer
#define PRI_I8 PRIi8

typedef uint8_t U8;  //!< 8-bit unsigned integer
#define PRI_U8 PRIu8

typedef U8 BYTE;  //!< byte type
#define PRI_BYTE PRIu8

typedef char CHAR;
#define PRI_CHAR "c"

#if FW_HAS_16_BIT
typedef int16_t I16;  //!< 16-bit signed integer
#define PRI_I16 PRIi16
typedef uint16_t U16;  //!< 16-bit unsigned integer
#define PRI_U16 PRIu16
#endif

#if FW_HAS_32_BIT
typedef int32_t I32;  //!< 32-bit signed integer
#define PRI_I32 PRIi32
typedef uint32_t U32;  //!< 32-bit unsigned integer
#define PRI_U32 PRIu32
#endif

#if FW_HAS_64_BIT
typedef int64_t I64;  //!< 64-bit signed integer
#define PRI_I64 PRIi64
typedef uint64_t U64;  //!< 64-bit unsigned integer
#define PRI_U64 PRIu64
#endif

typedef float F32;  //!< 32-bit floating point
#define PRI_F64 "lf"
typedef double F64;  //!< 64-bit floating point (double). Required for compiler-supplied double promotion.

/*----------------------------------------------------------------------------*/
/* Useful macro definitions                                                   */
/*----------------------------------------------------------------------------*/
#define FW_NUM_ARRAY_ELEMENTS(a) (sizeof(a) / sizeof((a)[0]))  //!< number of elements in an array
#define FW_MAX(a, b) (((a) > (b)) ? (a) : (b))                 //!< MAX macro
#define FW_MIN(a, b) (((a) < (b)) ? (a) : (b))                 //!< MIN macro

#define FW_NO_ASSERT 1  //!< Asserts turned off
#define FW_FILEID_ASSERT \
    2  //!< File ID used - requires -DASSERT_FILE_ID=somevalue to be set on the compile command line
#define FW_FILENAME_ASSERT 3  //!< Uses the file path in the assert - image stores filenames
#define FW_RELATIVE_PATH_ASSERT \
    4  //!< Uses a relative file path (within fprime/fprime library) for assert. - requires -DASSERT_RELATIVE_PATH=path
       //!< to be set on the compile command line

#ifdef __cplusplus
}
#endif
#endif  // FW_BASIC_TYPES_H
