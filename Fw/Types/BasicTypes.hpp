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

#include <FpConfig.hpp>
#include <StandardTypes.hpp> // This header will be found be include paths by target. This hides different header files for each target.
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// Define native integer/unsigned integer types
#ifdef _WRS_KERNEL
typedef int32_t NATIVE_INT_TYPE;
typedef uint32_t NATIVE_UINT_TYPE;
#else
typedef int NATIVE_INT_TYPE; //!< native integer type declaration
typedef unsigned int NATIVE_UINT_TYPE; //!< native unsigned integer type declaration
#endif

#if defined __GNUC__ || __llvm__

// This is used to cast pointers to integers
// when a pointer needs to be stored generically.
// In order to avoid chopping off bits,
// the integer bit size needs to match
// the pointer bit size.

#ifdef __SIZEOF_POINTER__
 #if __SIZEOF_POINTER__ == 8
  #define POINTER_CAST U64
 #elif __SIZEOF_POINTER__ == 4
  #define POINTER_CAST U32
 #elif __SIZEOF_POINTER__ == 2
  #define POINTER_CAST U16
 #else
  #define POINTER_CAST U8
 #endif
#elif __i386 == 1 // GCC 4.1.2
  #define POINTER_CAST U32
#elif __x86_64 == 1 // GCC 4.1.2
  #define POINTER_CAST U64
#elif CPU == PPC604 // VxWorks 6.7 RAD750
  #define POINTER_CAST U32
#elif CPU == SPARC
  #define POINTER_CAST U32
#else
 #error Cannot get size of pointer cast!
#endif

#else
 #error Unsupported compiler!
#endif

// compile-time assert
#define COMPILE_TIME_ASSERT( condition, name )\
  do { \
   enum { assert_failed_ ## name = 1/(condition) }; \
  } while(0)

/*----------------------------------------------------------------------------*/
typedef int8_t          I8; //!< 8-bit signed integer
typedef uint8_t         U8; //!< 8-bit unsigned integer
typedef U8              BYTE; //!< byte type

#if FW_HAS_16_BIT
 typedef int16_t        I16; //!< 16-bit signed integer
 typedef uint16_t       U16; //!< 16-bit unsigned integer
#endif

#if FW_HAS_32_BIT
 typedef uint32_t       U32; //!< 32-bit signed integer
 typedef int32_t        I32; //!< 32-bit unsigned integer
#endif

#if FW_HAS_64_BIT
 typedef int64_t        I64; //!< 64-bit signed integer
 typedef uint64_t       U64; //!< 64-bit unsigned integer
#endif
 
typedef float   F32; //!< 32-bit floating point
#if FW_HAS_F64
 typedef double  F64; //!< 64-bit floating point
#endif

#ifndef NULL
#define NULL  (0)  //!< NULL
#endif

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

// Not ideal, but VxWorks doesn't have strnlen
#ifdef __VXWORKS__
NATIVE_INT_TYPE strnlen(const char *s, NATIVE_INT_TYPE maxlen);
#endif

#ifdef __cplusplus
} // extern "C"

#endif // __cplusplus

#endif
