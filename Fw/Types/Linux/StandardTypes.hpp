/**
 * \brief StandardTypes.hpp for Linux/Darwin systems
 *
 * StandardTypes.hpp is typically published by platform developers to define
 * the standard available arithmetic types for use in fprime. This standard
 * types header is designed to support standard Linux/Darwin distributions
 * running on x86, x86_64 machines and using the standard gcc/clang compilers
 * shipped with the operating system.
 */
// Used to define the standard intX_t and uintX_t types
#include <cstdint>
#include <cinttypes>
#include <limits>

/**
 * DEPRECATED.
 *
 * This type is deprecated as it deals specifically with the ambiguously sized
 * "int" type.  It is used as a base definition for Linux/Darwin systems
 * but usages outside this file should be replaced with another named type found
 * below.
 */
typedef unsigned int PlatformUIntType;
const PlatformUIntType PlatformUIntType_MIN = std::numeric_limits<unsigned int>::min();
const PlatformUIntType PlatformUIntType_MAX = std::numeric_limits<unsigned int>::max();
#define PLATFORM_UINT_TYPE_DEFINED
#define PRI_PlatformUIntType PRI_unsigned int

/**
 * DEPRECATED.
 *
 * This type is deprecated as it deals specifically with the ambiguously sized
 * "int" type.  It is used as a base definition for Linux/Darwin systems
 * but usages outside this file should be replaced with another named type found
 * below.
 */
typedef int PlatformIntType;
const PlatformIntType PlatformIntType_MIN = std::numeric_limits<int>::min();;
const PlatformIntType PlatformIntType_MAX = std::numeric_limits<int>::max();;
#define PLATFORM_INT_TYPE_DEFINED
#define PRI_PlatformIntType PRI_int

/**
 * Type defined for linux/darwin for passing arguments to FW_ASSERT calls. All
 * arguments must be cast into this type and this type must be large enough to
 * hold all arguments without loss when casting.
 */
typedef PlatformIntType PlatformAssertArgType;
const PlatformAssertArgType PlatformAssertArgType_MIN = PlatformIntType_MIN;
const PlatformAssertArgType PlatformAssertArgType_MAX = PlatformIntType_MAX;
#define PLATFORM_ASSERT_ARG_TYPE_DEFINED
#define PRI_PlatformAssertArgType PRI_PlatformIntType

/**
 * Type defined for linux/darwin for dealing holding pointers cast as integers.
 */
typedef PlatformIntType PlatformPointerCastType;
const PlatformPointerCastType PlatformPointerCastType_MIN = PlatformIntType_MIN;
const PlatformPointerCastType PlatformPointerCastType_MAX = PlatformIntType_MAX;
#define PLATFORM_POINTER_CAST_TYPE_DEFINED
#define PRI_PlatformPointerCastType PRI_PlatformIntType




/**
 * Type defined for linux/darwin for dealing with sizes. Typically used for file
 * and memory sizes.
 */
typedef PlatformUIntType PlatformSizeType;
const PlatformSizeType PlatformSizeType_MIN = PlatformUIntType_MIN;
const PlatformSizeType PlatformSizeType_MAX = PlatformUIntType_MAX;
#define PLATFORM_SIZE_TYPE_DEFINED
#define PRI_PlatformSizeType PRI_PlatformUIntType

/**
 * Type defined for linux/darwin for dealing with ports indices
 */
typedef PlatformIntType PlatformIndexType;
const PlatformIndexType PlatformIndexType_MIN = PlatformIntType_MIN;
const PlatformIndexType PlatformIndexType_MAX = PlatformIntType_MAX;
#define PLATFORM_INDEX_TYPE_DEFINED
#define PRI_PlatformIndexType PRI_PlatformIntType



