# F´ Numerical Types

On physical hardware, numerical types have a size in bits. The sizes of these integers are either explicitly set by the
programmer or implicitly set by the compiler. i.e. `uint32_t` is explicitly 32-bits whereas `int` is implicitly set by
the compiler. Flight software practice encourages use of explicitly sized types because there are fewer mistake made when the
programmer uses an integer of a known size. However, in-practice it is nice to also use logical types that represent
concepts in the system (e.g. size) and are set by the platform supporter to a known size.

This document describes: fixed-width types and logical types.

## Fixed Width Types

In F´, fixed width types map to the standard definitions either in the C standard or in the `stdint.h` header as seen
below. Since compilers are not guaranteed to support all types (e.g. 64bits integers) these types can be turned off
by setting a configuration field to `0` in `FpConfig.hpp`.  These types are by default on and users must turn off types
their compiler does not support.


| F´ Type | Equivalent   | `FpConfig.hpp` Configuration Field |
|---------|--------------|------------------------------------|
| I8      | int8_t       | n/a                                |
| I16     | int16_t      | FW_HAS_16_BIT                      |
| I32     | int32_t      | FW_HAS_32_BIT                      |
| I64     | int64_t      | FW_HAS_64_BIT                      |
| U8      | uint8_t      | n/a                                |
| U16     | uint16_t     | FW_HAS_16_BIT                      |
| U32     | uint32_t     | FW_HAS_32_BIT                      |
| U64     | uint64_t     | FW_HAS_64_BIT                      |
| F32     | float        | n/a                                |
| F64     | double       | FW_HAS_F64                         |

Platform developers should include `stdint.h` or equivalent in their `PlatformTypes.hpp` file provided alongside their
platform. If for some reason that header does not exist or does not define all types, then developers must define all
"Equivalent" definitions above.

To print these types, users may use the standard C++ PRI macros as shown described:
[https://cplusplus.com/reference/cinttypes/](https://cplusplus.com/reference/cinttypes/).

```c++
U32 value = 10;
printf("My value: %" PRId32, value); // Uses string constant concatenation
```

Minimum and maximum limits are provided by `FpLimits` of the form `FpLimits::<type>_MIN` and `FpLimits::<type>_MAX`.

## F´ Logical Integer Type Design


Typically, in flight software we try to use fixed-sized types wherever possible as this produces consistent results
across platforms. However, when building components for use on multiple different architectures it is nice to be able
to use logical integer types whose exact widths are determined by the platform and project instead of needing
to pick  a one-size-fits-all fixed with type. For example, representing a size as an unsigned 64-bit integer is logical
on some systems that have 64-bit data widths, but will not compile on systems that do not define 64-bit integers at all
(8-bit systems often lack this data size).

Thus, for these logical types the actual data size needs to be configured. This configuration needs to be available
from two sources: a given project, and a given platform. Platforms need to set the idea size for these types, and
projects need to be able to control these sizes when operating across multiple platforms.

### Platform Configured Types

Platform developers must define the following logical types in the `PlatformTypes.hpp` header provided alongside
their CMake platform and toolchain files. Each type also defines a format specifier for use with the `printf` family of
functions. Additionally, each defines a pair of static constants of the form `<type>_MIN` and `<type>_MAX` as limits to
the range of these values. All limits must be defined as public static constants or public static constant expressions
within a struct called `PlatformLimits`. For simplicity, structs are used to ensure all definitions are public.

| Platform Logical Type   | Format Specifier            | Notes                       | 
|-------------------------|-----------------------------|-----------------------------|
| PlatformIndexType       | PRI_PlatformIndexType       | Ports indices               | 
| PlatformSizeType        | PRI_PlatformSizeType        | Sizes                       |
| PlatformPointerCastType | PRI_PlatformPointerCastType | Pointers stored as integers |
| PlatformAssertArgType   | PRI_PlatformAssertArgType   | Argument to FW_ASSERT       |
| PlatformIntType         | PRI_PlatformIntType         | Deprecated (see note)       |
| PlatformUIntType        | PRI_PlatformUIntType        | Deprecated (see note)       |

A complete definition of each type for a given platform must be supplied within `PlatformTypes.hpp` as shown in the
example below. Notice the type is defined along with a format specifier and static constants are set in the
`PlatformLimits` struct.

```c++
typedef int32_t PlatformIndexType;
#define PRI_PlatformIndexType PRId32
...

struct PlatformLimits {
    ...
    static const PlatformIndexType PlatformIndexType_MIN = 0;
    static const PlatformIndexType PlatformIndexType_MAX = INT32_MAX;
    ...
};
```

In order to print these types, developers can use the following example as a basis for using the PRI_* macros. This
example also shows the use of the type's minimum limit.

```c++
PlatformIndexType index = 3;
// Print the above index type, and the minimum value supported by the same type
printf("Index %" PRI_PlatformIndexType " is bound by %" PRI_PlatformIndexType, index, FpLimits::PlatformIndexType_MIN);
```

**Note:** in order for F´ to compile without warnings it is necessary that each of the platform types are elements in
the set of integers supplied by the C standard int header. i.e. each type is an `int8_t`, `int16_t`, `int32_t`,
`int64_t` or unsigned equivalents. On some compilers `int` and `unsigned int` are not members of that set and on those
platforms it is imperative that both `PlatformIntType` and `PlatformUIntType` be set to some fixed size type instead.

Additionally, types are defined as public static const expressions within a struct for two reasons. First, it allows the
use of strongly typed constants without allocating storage for the constant nor requiring compiler optimization to
remove storage. Second, it allows inheritance such that all constants from `PlatformLimits`, `BasicLimits`, and
`FpLimits` (described below) are available through `FpLimits`. This implementation implies that one should never take
the address of one of these constants.

### Configurable Integer Types

Project may configure the framework types that the framework and components use for implementation through
`FpConfig.hpp`. The default configuration as supplied with F´uses the above platform types where applicable.
`<type>_MIN` and `<type>_MAX` limit constants are to be defined publicly within the `FpLimits` struct. The `FpLimits`
struct inherit from `BasicLimits` using private inheritance as shown in the example at the bottom of this section.

| Framework Type  | Logical Usage        | Default               | Format Specifier    | Notes |
|-----------------|----------------------|-----------------------|---------------------|-------|
| FwIndexType     | Port indices         | PlatformIndexType     | PRI_FwIndexType     |       |
| FwSizeType      | Sizes                | PlatformSizeType      | PRI_FwSizeType      |       |
| FwAssertArgType | Arguments to asserts | PlatformAssertArgType | PRI_FwAssertArgType |       |

There is also a set of framework types that are used across F´ deployments and specifically interact with ground data
systems. These GDS types have defaults based on configurable platform independent fixed-widths as shown below:

| GDS Type               | Logical Usage              | Default               | Format Specifier           |
|------------------------|----------------------------|-----------------------|----------------------------|
| FwBuffSizeType         | `Fw::Buffer` sizes         | U16                   | PRI_FwBuffSizeType         |
| FwEnumStoreType        | Enumeration values         | I32                   | PRI_FwEnumStoreType        |
| FwTimeBaseStoreType    | Time base                  | U16                   | PRI_FwTimeBaseStoreType    |
| FwTimeContextStoreType | Time context               | U8                    | PRI_FwTimeContextStoreType |
| FwPacketDescriptorType | F´ packet descriptor field | U32                   | PRI_FwPacketDescriptorType |
| FwOpcodeType           | F´ command opcodes         | U32                   | PRI_FwOpcodeType           |
| FwChanIdType           | F´ channel ids             | U32                   | PRI_FwChanIdType           |
| FwEventIdType          | F´ event ids               | U32                   | PRI_FwEventIdType          |
| FwPrmIdType            | F´ parameter ids           | U32                   | PRI_FwPrmIdType            |
| FwTlmPacketizeIdType   | F´ telemetry packet ids    | U16                   | PRI_FwTlmPacketizeIdType   |

All defaults can be overridden via project specific configuration supplying a custom `FpConfig.hpp`. A complete
definition of a framework/GDS type in `FpConfig.hpp` would look like:

```c++
#include <BasicTypes.hpp>
...
typedef uint32_t FwSizeType;
#define PRI_FwSizeType PRIu32
...

struct FpLimits : BasicLimits {
    const FwSizeType PlatformIndexType_MIN = 0;
    const FwSizeType PlatformIndexType_MAX = UINT32_MAX;
};
```
