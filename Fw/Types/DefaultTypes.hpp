/**
 * \brief DefaultTypes.hpp provides fallback defaults for the platform types
 *
 * This fill contains default implementations for types typically defined in
 * PlatformTypes.hpp. These default implementations are based on x86_64 Linux
 * but are often appropriate for most systems
 */
#include <limits>
/**
* Default implementation for deprecated (see note)
 */
#ifndef PLATFORM_INT_TYPE_DEFINED
typedef int PlatformIntType;
const PlatformIntType PlatformIntType_MIN = std::numeric_limits<int>::min();
const PlatformIntType PlatformIntType_MAX = std::numeric_limits<int>::max();
#define PLATFORM_INT_TYPE_DEFINED
#define PRI_PlatformIntType "d"
#endif


/**
* Default implementation for deprecated (see note)
 */
#ifndef PLATFORM_UINT_TYPE_DEFINED
typedef unsigned int PlatformUIntType;
const PlatformUIntType PlatformUIntType_MIN = std::numeric_limits<unsigned int>::min();
const PlatformUIntType PlatformUIntType_MAX = std::numeric_limits<unsigned int>::max();
#define PLATFORM_UINT_TYPE_DEFINED
#define PRI_PlatformUIntType "ud"
#endif

/**
* Default implementation for ports indices
*/
#ifndef PLATFORM_INDEX_TYPE_DEFINED
typedef PlatformIntType PlatformIndexType;
const PlatformIndexType PlatformIndexType_MIN = PlatformIntType_MIN;
const PlatformIndexType PlatformIndexType_MAX = PlatformIntType_MAX;
#define PLATFORM_INDEX_TYPE_DEFINED
#define PRI_PlatformIndexType PRI_PlatformIntType
#endif

/**
* Default implementation for sizes
*/
#ifndef PLATFORM_SIZE_TYPE_DEFINED
typedef PlatformUIntType PlatformSizeType;
const PlatformSizeType PlatformSizeType_MIN = PlatformUIntType_MIN;
const PlatformSizeType PlatformSizeType_MAX = PlatformUIntType_MAX;
#define PLATFORM_SIZE_TYPE_DEFINED
#define PRI_PlatformSizeType PRI_PlatformUIntType
#endif

/**
* Default implementation for pointers stored as integers
*/
#ifndef PLATFORM_POINTER_CAST_TYPE_DEFINED
typedef uint64_t PlatformPointerCastType;
const PlatformPointerCastType PlatformPointerCastType_MIN = std::numeric_limits<uint64_t>::min();
const PlatformPointerCastType PlatformPointerCastType_MAX = std::numeric_limits<uint64_t>::max();
#define PLATFORM_POINTER_CAST_TYPE_DEFINED
#define PRI_PlatformPointerCastType PRIx64
#endif

/**
* Default implementation for argument to fw_assert
*/
#ifndef PLATFORM_ASSERT_ARG_TYPE_DEFINED
typedef PlatformIntType PlatformAssertArgType;
const PlatformAssertArgType PlatformAssertArgType_MIN = PlatformIntType_MIN;
const PlatformAssertArgType PlatformAssertArgType_MAX = PlatformIntType_MAX;
#define PLATFORM_ASSERT_ARG_TYPE_DEFINED
#define PRI_PlatformAssertArgType PRI_PlatformIntType
#endif

