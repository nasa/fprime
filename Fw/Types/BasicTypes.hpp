/**
 * \file
 * \author T. Canham
 * \brief Declares ISF basic types
 *
 * \copyright
 * Copyright 2009-2016, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 *
 */

#ifndef FW_BASIC_TYPES_HPP
#define FW_BASIC_TYPES_HPP

#include <StandardTypes.hpp>
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#if defined __GNUC__ || __llvm__
#else
  #error Unsupported compiler!
#endif

/*----------------------------------------------------------------------------*/
typedef int8_t          I8; //!< 8-bit signed integer
typedef uint8_t         U8; //!< 8-bit unsigned integer
typedef U8              BYTE; //!< byte type
typedef char CHAR;

const int8_t I8_MIN = INT8_MIN;
const int8_t I8_MAX = INT8_MAX;

const uint8_t U8_MIN = 0;
const uint8_t U8_MAX = UINT8_MAX;

#if FW_HAS_16_BIT
    typedef int16_t        I16; //!< 16-bit signed integer
    typedef uint16_t       U16; //!< 16-bit unsigned integer
    const I16 I16_MIN = INT16_MIN;
    const I16 I16_MAX = INT16_MAX;
    const U16 U16_MIN = 0;
    const U16 U16_MAX = UINT16_MAX;
#endif

#if FW_HAS_32_BIT
  typedef uint32_t       U32; //!< 32-bit signed integer
  typedef int32_t        I32; //!< 32-bit unsigned integer
  const I32 I32_MIN = INT32_MIN;
  const I32 I32_MAX = INT32_MAX;
  const U32 U32_MIN = 0;
  const U32 U32_MAX = UINT32_MAX;
#endif

#if FW_HAS_64_BIT
  typedef int64_t        I64; //!< 64-bit signed integer
  typedef uint64_t       U64; //!< 64-bit unsigned integer
  const I64 I64_MIN = INT64_MIN;
  const I64 I64_MAX = INT64_MAX;
  const U64 U64_MIN = 0;
  const U64 U64_MAX = UINT64_MAX;
#endif

typedef float   F32; //!< 32-bit floating point
#if FW_HAS_F64
  typedef double  F64; //!< 64-bit floating point
#endif

  // compile-time assert
#define COMPILE_TIME_ASSERT( condition, name )\
  do { \
   enum { assert_failed_ ## name = 1/(condition) }; \
  } while(0)


#define FW_NUM_ARRAY_ELEMENTS(a)  (sizeof(a)/sizeof((a)[0])) //!< number of elements in an array

#define FW_MAX(a,b) (((a) > (b))?(a):(b)) //!< MAX macro
#define FW_MIN(a,b) (((a) < (b))?(a):(b)) //!< MIN macro

// STATIC for C builds
#ifndef STATIC
#define STATIC static //!< static for non unit-test code
#endif

#ifndef PROTECTED
#define PROTECTED protected //!< overridable protected for unit testing
#endif

#ifndef PRIVATE
#define PRIVATE private //!< overridable private for unit testing
#endif

#ifdef __cplusplus
} // extern "C"

#endif // __cplusplus

#endif
