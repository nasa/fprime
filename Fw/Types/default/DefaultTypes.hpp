/**
 * \brief DefaultTypes.hpp provides fallback defaults for the platform types
 *
 * This fill contains default implementations for types typically defined in
 * PlatformTypes.hpp. These default implementations are based on x86_64 Linux
 * but are often appropriate for most systems.
 */
#include <limits>
/**
* Default implementation for deprecated (see note)
 */
#ifndef PLATFORM_INT_TYPE_DEFINED
typedef int PlatformIntType;
extern const PlatformIntType PlatformIntType_MIN;
extern const PlatformIntType PlatformIntType_MAX;
#define PLATFORM_INT_TYPE_DEFINED
#define PRI_PlatformIntType "d"
#endif


/**
* Default implementation for deprecated (see note)
 */
#ifndef PLATFORM_UINT_TYPE_DEFINED
typedef unsigned int PlatformUIntType;
extern const PlatformUIntType PlatformUIntType_MIN;
extern const PlatformUIntType PlatformUIntType_MAX;
#define PLATFORM_UINT_TYPE_DEFINED
#define PRI_PlatformUIntType "ud"
#endif

/**
* Default implementation for ports indices
*/
#ifndef PLATFORM_INDEX_TYPE_DEFINED
typedef PlatformIntType PlatformIndexType;
extern const PlatformIndexType PlatformIndexType_MIN;
extern const PlatformIndexType PlatformIndexType_MAX;
#define PLATFORM_INDEX_TYPE_DEFINED
#define PRI_PlatformIndexType PRI_PlatformIntType
#endif

/**
* Default implementation for sizes
*/
#ifndef PLATFORM_SIZE_TYPE_DEFINED
typedef PlatformUIntType PlatformSizeType;
extern const PlatformSizeType PlatformSizeType_MIN;
extern const PlatformSizeType PlatformSizeType_MAX;
#define PLATFORM_SIZE_TYPE_DEFINED
#define PRI_PlatformSizeType PRI_PlatformUIntType
#endif

/**
* Default implementation for argument to fw_assert
*/
#ifndef PLATFORM_ASSERT_ARG_TYPE_DEFINED
typedef PlatformIntType PlatformAssertArgType;
extern const PlatformAssertArgType PlatformAssertArgType_MIN;
extern const PlatformAssertArgType PlatformAssertArgType_MAX;
#define PLATFORM_ASSERT_ARG_TYPE_DEFINED
#define PRI_PlatformAssertArgType PRI_PlatformIntType
#endif

/**
* Default implementation for pointers stored as integers
*/
#ifndef PLATFORM_POINTER_CAST_TYPE_DEFINED
  // Check for __SIZEOF_POINTER__ or cause error
  #ifndef __SIZEOF_POINTER__
    #error "Compiler does not support __SIZEOF_POINTER__, cannot use default for PlatformPointerCastType"
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
  #define PLATFORM_POINTER_CAST_TYPE_DEFINED
#endif
