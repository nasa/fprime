/**
 * \brief PlatformTypes.hpp for Linux/Darwin systems
 *
 * PlatformTypes.hpp is typically published by platform developers to define
 * the standard available arithmetic types for use in fprime. This standard
 * types header is designed to support standard Linux/Darwin distributions
 * running on x86, x86_64 machines and using the standard gcc/clang compilers
 * shipped with the operating system.
 */

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

// No custom definitions, inherit everything from DefaultTypes.hpp

// Section 2: (optional) Default Types
//    for types not explicitly, defined in section 1, platform developers can
//    use DefaultTypes.hpp to get the defaults as described in the documentation
#include <Fw/Types/DefaultTypes.hpp>


