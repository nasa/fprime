/**
 * \brief PlatformTypes.hpp for Linux/Darwin systems
 *
 * PlatformTypes.hpp is typically published by platform developers to define
 * the standard available arithmetic types for use in fprime. This standard
 * types header is designed to support standard Linux/Darwin distributions
 * running on x86, x86_64 machines and using the standard gcc/clang compilers
 * shipped with the operating system.
 */
#include <limits>

// Section 0: C Standard Types
//    fprime depends on the existence of intN_t and uintN_t C standard ints and
//    the mix/max values for those types. Platform developers must either:
//    1. define these types and definitions
//    2. include headers that define these types
//
// In addition, support for various type widths can be turned on/off with the
// in this section to control what of the C standard types are available in the
// system. fprime consumes this information and produces the UN, IN, and FN
// types we see in fprime code
#include <cstdint>
#include <cinttypes>
#include <limits>

#define FW_HAS_64_BIT    1 //!< Architecture supports 64 bit integers
#define FW_HAS_32_BIT    1 //!< Architecture supports 32 bit integers
#define FW_HAS_16_BIT    1 //!< Architecture supports 16 bit integers
#define FW_HAS_F64    1 //!< Architecture supports 64 bit floating point numbers

// Section 1: Logical Types
//    fprime requires platform implementors to define logical types for their
//    system. The list of logical types can be found in the document:
//    docs/Design/numerical-types.md with the names of the form "Platform*"

typedef int PlatformIntType;
extern const PlatformIntType PlatformIntType_MIN;
extern const PlatformIntType PlatformIntType_MAX;
#define PRI_PlatformIntType "d"

typedef unsigned int PlatformUIntType;
extern const PlatformUIntType PlatformUIntType_MIN;
extern const PlatformUIntType PlatformUIntType_MAX;
#define PRI_PlatformUIntType "ud"

typedef PlatformIntType PlatformIndexType;
extern const PlatformIndexType PlatformIndexType_MIN;
extern const PlatformIndexType PlatformIndexType_MAX;
#define PRI_PlatformIndexType PRI_PlatformIntType

typedef PlatformUIntType PlatformSizeType;
extern const PlatformSizeType PlatformSizeType_MIN;
extern const PlatformSizeType PlatformSizeType_MAX;
#define PRI_PlatformSizeType PRI_PlatformUIntType

typedef PlatformIntType PlatformAssertArgType;
extern const PlatformAssertArgType PlatformAssertArgType_MIN;
extern const PlatformAssertArgType PlatformAssertArgType_MAX;
#define PRI_PlatformAssertArgType PRI_PlatformIntType

// Linux/Darwin definitions for pointer have various sizes
#ifndef PLATFORM_POINTER_CAST_TYPE_DEFINED
  // Check for __SIZEOF_POINTER__ or cause error
  #ifndef __SIZEOF_POINTER__
    #error "Compiler does not support __SIZEOF_POINTER__, cannot use Linux/Darwin types"
  #endif

  // Pointer sizes are determined by size of compiler
  #if __SIZEOF_POINTER__ == 8
    typedef uint64_t PlatformPointerCastType;
    extern const PlatformPointerCastType PlatformPointerCastType_MIN;
    extern const PlatformPointerCastType PlatformPointerCastType_MAX;
    #define PRI_PlatformPointerCastType PRIx64
  #elif __SIZEOF_POINTER__ == 4
    typedef uint32_t PlatformPointerCastType;
    extern const PlatformPointerCastType PlatformPointerCastType_MIN;
    extern const PlatformPointerCastType PlatformPointerCastType_MAX;
    #define PRI_PlatformPointerCastType PRIx32
  #elif __SIZEOF_POINTER__ == 2
    typedef uint16_t PlatformPointerCastType;
    extern const PlatformPointerCastType PlatformPointerCastType_MIN;
    extern const PlatformPointerCastType PlatformPointerCastType_MAX;
    #define PRI_PlatformPointerCastType PRIx16
  #else
    typedef uint8_t PlatformPointerCastType;
    extern const PlatformPointerCastType PlatformPointerCastType_MIN;
    extern const PlatformPointerCastType PlatformPointerCastType_MAX;
    #define PRI_PlatformPointerCastType PRIx8
  #endif
#endif
