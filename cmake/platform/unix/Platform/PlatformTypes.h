
/**
 * \brief PlatformTypes.h C-compatible type definitions for Linux/Darwin
 *
 * PlatformTypes.h is typically published by platform developers to define
 * the standard available arithmetic types for use in fprime. This standard
 * types header is designed to support standard Linux/Darwin (unix) distributions
 * running on x86, x86_64, arm, and arm64 machines and using the standard gcc/clang
 * compilers shipped with the operating system.
 */
#ifndef PLATFORM_TYPES_H_
#define PLATFORM_TYPES_H_

#ifdef  __cplusplus
extern "C" {
#endif
#include <stdint.h>

// Linux/Darwin definitions for pointer have various sizes across platforms
// and since these definitions need to be consistent we must ask the size.
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

#ifdef  __cplusplus
}
#endif

#endif  // PLATFORM_TYPES_H_
