/**
 * \brief PlatformTypes.h C-compatible type definitions for Linux/Darwin
 *
 * PlatformTypes.h is typically published by platform developers to define
 * the standard available arithmetic types for use in fprime. This standard
 * types header is designed to support standard Linux/Darwin distributions
 * running on x86, x86_64 machines and using the standard gcc/clang compilers
 * shipped with the operating system.
 *
 * In C++ code, users may use std::numeric_limits<PlatformIntType>::min()
 * to reference the min/max limits of a type.
 */
#ifndef PLATFORM_TYPES_H_
#define PLATFORM_TYPES_H_

// Section 0: C Standard Types
//    fprime depends on the existence of intN_t and uintN_t C standard ints and
//    the mix/max values for those types. Platform developers must either:
//    1. define these types and definitions
//    2. include headers that define these types
//
// In addition, support for various type widths can be turned on/off with the
// switches in this section to control which of the C standard types are
// available in the system. fprime consumes this information and produces the
// UN, IN, and FN types we see in fprime code.
#include <inttypes.h>
#include <stdint.h>

// Define what types and checks are supported by this platform
#define FW_HAS_64_BIT 1                   //!< Architecture supports 64 bit integers
#define FW_HAS_32_BIT 1                   //!< Architecture supports 32 bit integers
#define FW_HAS_16_BIT 1                   //!< Architecture supports 16 bit integers
#define FW_HAS_F64 1                      //!< Architecture supports 64 bit floating point numbers
#define SKIP_FLOAT_IEEE_754_COMPLIANCE 0  //!<  Check IEEE 754 compliance of floating point arithmetic

// Section 1: Logical Types
//    fprime requires platform implementors to define logical types for their
//    system. The list of logical types can be found in the document:
//    docs/Design/numerical-types.md with the names of the form "Platform*"

typedef int PlatformIntType;
#define PRI_PlatformIntType "d"

typedef unsigned int PlatformUIntType;
#define PRI_PlatformUIntType "u"

typedef PlatformIntType PlatformIndexType;
#define PRI_PlatformIndexType PRI_PlatformIntType

typedef PlatformUIntType PlatformSizeType;
#define PRI_PlatformSizeType PRI_PlatformUIntType

typedef PlatformIntType PlatformAssertArgType;
#define PRI_PlatformAssertArgType PRI_PlatformIntType

// Linux/Darwin definitions for pointer have various sizes across platforms
// and since these definitions need to be consistent we must ask the size.
#ifndef PLATFORM_POINTER_CAST_TYPE_DEFINED
// Check for __SIZEOF_POINTER__ or cause error
#ifndef __SIZEOF_POINTER__
#error "Compiler does not support __SIZEOF_POINTER__, cannot use Linux/Darwin types"
#endif

// Pointer sizes are determined by compiler
#if __SIZEOF_POINTER__ == 8
typedef uint64_t PlatformPointerCastType;
#define PRI_PlatformPointerCastType PRIx64
#elif __SIZEOF_POINTER__ == 4
typedef uint32_t PlatformPointerCastType;
#define PRI_PlatformPointerCastType PRIx32
#elif __SIZEOF_POINTER__ == 2
typedef uint16_t PlatformPointerCastType;
#define PRI_PlatformPointerCastType PRIx16
#elif __SIZEOF_POINTER__ == 1
typedef uint8_t PlatformPointerCastType;
#define PRI_PlatformPointerCastType PRIx8
#else
#error "Expected __SIZEOF_POINTER__ to be one of 8, 4, 2, or 1"
#endif
#endif
#endif  // PLATFORM_TYPES_H_
