/**
 * \brief PlatformTypes.hpp for Linux/Darwin systems
 *
 * PlatformTypes.hpp is typically published by platform developers to define
 * the standard available arithmetic types for use in fprime. This standard
 * types header is designed to support standard Linux/Darwin distributions
 * running on x86, x86_64 machines and using the standard gcc/clang compilers
 * shipped with the operating system.
 *
 * As part of these definitions, we include min and max constants for each
 * type. Implementors must define these constants in a C++ struct for two
 * reasons:
 *
 *   1. static constants in structs do not incur storage when referred to
 *      in a static context without the need for optimization in gcc
 *
 *   2. static constants in structs can be inherited via private inheritance
 *      allowing the definitions to be available to descendant functions
 *
 * In C++ fprime code, users may then refer to FpLimits::PlatformIntType_MIN
 * without referring to this header directly.
 */
#ifndef PLATFORM_TYPES_HPP_
#define PLATFORM_TYPES_HPP_
#include <limits>

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
#define PRI_PlatformIntType "d"

typedef unsigned int PlatformUIntType;
#define PRI_PlatformUIntType "ud"

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

  // Pointer sizes are determined by size of compiler
  #if __SIZEOF_POINTER__ == 8
    typedef uint64_t PlatformPointerCastType;
    #define PRI_PlatformPointerCastType PRIx64
  #elif __SIZEOF_POINTER__ == 4
    typedef uint32_t PlatformPointerCastType;
    #define PRI_PlatformPointerCastType PRIx32
  #elif __SIZEOF_POINTER__ == 2
    typedef uint16_t PlatformPointerCastType;
    #define PRI_PlatformPointerCastType PRIx16
  #else
    typedef uint8_t PlatformPointerCastType;
    #define PRI_PlatformPointerCastType PRIx8
  #endif
#endif
/**
 * PlatformLimits:
 *
 * PlatformLimits define the min and max values for the constructs defined
 * within this file. These must be defined as `static const` members to
 * ensure that unnecessary storage is not allocated.
 */
struct PlatformLimits {
    static const PlatformIntType PlatformIntType_MIN = std::numeric_limits<int>::min();
    static const PlatformIntType PlatformIntType_MAX = std::numeric_limits<int>::max();

    static const PlatformUIntType PlatformUIntType_MIN = std::numeric_limits<unsigned int>::min();
    static const PlatformUIntType PlatformUIntType_MAX = std::numeric_limits<unsigned int>::max();

    static const PlatformIndexType PlatformIndexType_MIN = PlatformIntType_MIN;
    static const PlatformIndexType PlatformIndexType_MAX = PlatformIntType_MAX;

    static const PlatformSizeType PlatformSizeType_MIN = PlatformUIntType_MIN;
    static const PlatformSizeType PlatformSizeType_MAX = PlatformUIntType_MAX;

    static const PlatformAssertArgType PlatformAssertArgType_MIN = PlatformIntType_MIN;
    static const PlatformAssertArgType PlatformAssertArgType_MAX = PlatformIntType_MAX;

    // Pointer sizes are determined by size of compiler
    #if __SIZEOF_POINTER__ == 8
        static const PlatformPointerCastType PlatformPointerCastType_MIN = std::numeric_limits<uint64_t>::min();
        static const PlatformPointerCastType PlatformPointerCastType_MAX = std::numeric_limits<uint64_t>::max();
    #elif __SIZEOF_POINTER__ == 4
        static const PlatformPointerCastType PlatformPointerCastType_MIN = std::numeric_limits<uint32_t>::min();
        static const PlatformPointerCastType PlatformPointerCastType_MAX = std::numeric_limits<uint32_t>::max();
    #elif __SIZEOF_POINTER__ == 2
        static const PlatformPointerCastType PlatformPointerCastType_MIN = std::numeric_limits<uint16_t>::min();
        static const PlatformPointerCastType PlatformPointerCastType_MAX = std::numeric_limits<uint16_t>::max();
    #elif __SIZEOF_POINTER__ == 1
        static const PlatformPointerCastType PlatformPointerCastType_MIN = std::numeric_limits<uint8_t>::min();
        static const PlatformPointerCastType PlatformPointerCastType_MAX = std::numeric_limits<uint8_t>::max();
    #else
        #error "Unsupported pointer size"
    #endif
};
#endif //PLATFORM_TYPES_HPP_
