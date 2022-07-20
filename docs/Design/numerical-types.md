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

Platform developers should include `stdint.h` or equivalent in their `StandardTypes.hpp` file provided alongside their
platform. If for some reason that header does not exist or does not define all types, then developers must define all
"Equivalent" definitions above.

To print these types, users may use the standard C++ PRI macros as shown described:
[https://cplusplus.com/reference/cinttypes/](https://cplusplus.com/reference/cinttypes/).

```c++
U32 value = 10;
printf("My value: %" PRId32, value); // Uses string constant concatenation
```

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

Platform developers should define the following logical types in the `StandardTypes.hpp` header provided alongside
their CMake platform and toolchain files. Each must also define a compiler directive to indicate the type was defined.
If the type is not specified as seen through the compiler directive the `DefaultTypes.hpp` header will fill in a default
definition. Each also defines a format specifier for use with the `printf` family of functions. Additionally, each
defines a pair of constants of the form `<type>_MIN` and `<type>_MAX` to define the minimum and maximum values.

| Platform Logical Type   | Compiler Directive                 | Default          | Format Specifier      | Notes                       | 
|-------------------------|------------------------------------|------------------|-----------------------|-----------------------------|
| PlatformIndexType       | PLATFORM_INDEX_TYPE_DEFINED        | PlatformIntType  | PRI_PlatformIndexType | Ports indices               | 
| PlatformSizeType        | PLATFORM_SIZE_TYPE_DEFINED         | PlatformUIntType | PRI_PlatformSizeType  | Sizes                       |
| PlatformPointerCastType | PLATFORM_POINTER_CAST_TYPE_DEFINED | PlatformIntType  | PRI_PointerCastType   | Pointers stored as integers |
| PlatformAssertArgType   | PLATFORM_ASSERT_ARG_TYPE_DEFINED   | PlatformIntType  | PRI_AssertArgType     | Argument to FW_ASSERT       |
| PlatformIntType         | PLATFORM_INT_TYPE_DEFINED          | int              | PRI_PlatformIntType   | Deprecated (see note)       |
| PlatformUIntType        | PLATFORM_UINT_TYPE_DEFINED         | unsigned int     | PRI_PlatformUIntType  | Deprecated (see note)       |

A complete definition of a type as a platform should supply within `StandardTypes.hpp` is shown below. Notice the type
is defined along with a compiler directive announcing it is defined, and a format specifier.

```c++
typedef int32_t PlatformIndexType;
const PlatformIndexType PlatformIndexType_MIN = 0;
const PlatformIndexType PlatformIndexType_MAX = INT32_MAX;
#define PLATFORM_INDEX_TYPE_DEFINED
#define PRI_PlatformIndexType PRId32
```

In order to print these types, developers can use the following example as a basis:

```c++
PlatformIndexType index = 3;
printf("Index: %" PRI_PlatformIndexType " is the index", index); // Uses string constant concatenation
```

**Note:** in order for F´ to compile without warnings it is necessary that each of the platform types are elements in
the set of integers supplied by the C standard int header. i.e. each type is an `int8_t`, `int16_t`, `int32_t`,
`int64_t` or unsigned equivalents. On some compilers `int` and `unsigned int` are not members of that set and on those
platforms it is imperative that both `PlatformIntType` and `PlatformUIntType` be set to some fixed size type instead.

### Configurable Integer Types

Project may configure the framework types that the framework and components use for implementation through
`FpConfig.hpp`. The default configuration uses the above platform types where applicable. `<type>_MIN` and `<type>_MAX`
to define the minimum and maximum values.

| Framework Type         | Logical Usage              | Default               | Format Specifier     | Notes      |
|------------------------|----------------------------|-----------------------|----------------------|------------|
| FwIndexType            | Port indices               | PlatformIndexType     | PRI_FwIndexType      |            | 
| FwSizeType             | Sizes                      | PlatformSizeType      | PRI_FwSizeType       |            |
| FwAssertArg            | Arguments to asserts       | PlatformAssertArgType | PRI_FwAssertArgType  |            |
| FwNativeIntType        | `int`                      | PlatformIntType       | PRI_FwNativeIntType  | Deprecated |
| FwNativeUIntType       | `unsigned int`             | PlatformUIntType      | PRI_FwNativeUIntType | Deprecated |

There is also a set of framework types that are used across F´ deployments and specifically interact with ground data
systems. `<type>_MIN` and `<type>_MAX` to define the minimum and maximum values. These GDS types are based on
configurable platform independent fixed-widths as shown below:

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

A complete definition of a framework/GDS type in `FpConfig.hpp` would look like:

```c++
typedef uint32_t FwSizeType;
const FwSizeType PlatformIndexType_MIN = 0;
const FwSizeType PlatformIndexType_MAX = UINT32_MAX;
#define PRI_FwSizeType PRIu32
```
