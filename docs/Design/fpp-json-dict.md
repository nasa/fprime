# FPP JSON Dictionary Specification
This document describes the format of FPP JSON dictionaries.

## Contents
- [Type Descriptors](#type-descriptors)
  - [Primitive Integer Type Descriptors](#primitive-integer-type-descriptors)
    - [Unsigned Integer Types](#unsigned-integer-types)
    - [Signed Integer Types](#signed-integer-types)
  - [Floating-Point Type Descriptors](#floating-point-type-descriptors)
    - [Floating-Point Types](#floating-point-types)
  - [Boolean Type Descriptors](#boolean-type-descriptors)
    - [Boolean Types](#boolean-types)
  - [String Type Descriptors](#string-type-descriptors)
    - [String Types](#string-types)
  - [Qualified Identifier Type Descriptors](#qualified-identifier-type-descriptors)
- [Type Definitions](#type-definitions)
  - [Array Type Definition](#array-type-definition)
  - [Enumeration Type Definition](#enumeration-type-definition)
  - [Struct Type Definition](#struct-type-definition)
    - [Struct Member](#struct-member)
    - [Struct Type Definition](#struct-type-definition-1)
- [Values](#values)
  - [Primitive Integer Values](#primitive-integer-values)
  - [Floating-Point Values](#floating-point-values)
  - [Boolean Values](#boolean-values)
  - [String Values](#string-values)
  - [Array Values](#array-values)
  - [Enumeration Values](#enumeration-values)
  - [Struct Values](#struct-values)
  - [Invalid Values](#invalid-values)
    - [Null Values](#null-values)
    - [Infinity Values](#infinity-values)
    - [Negative Infinity Values](#negative-infinity-values)
- [Commands, Telemetry Channels, Events, and Parameters](#commands-telemetry-channels-events-and-parameters)
  - [Formal Parameters](#formal-parameters)
  - [Commands](#commands)
  - [Telemetry Channels](#telemetry-channels)
  - [Events](#events)
  - [Parameters](#parameters)
- [Data Products](#data-products)
  - [Records](#records)
  - [Containers](#containers)
- [Dictionaries](#dictionaries)
  - [Dictionary Metadata](#dictionary-metadata)
  - [Dictionary Content](#dictionary-content)

# Type Descriptors

A _Type Descriptor_ is a JSON Dictionary that describes a type.

## Primitive Integer Type Descriptors
| Field | Description | Options | Required | 
| ----- | ----------- | ------- | -------- |
| `name` | **String** representing the FPP type name |  `U8`, `U16`, `U32`, `U64`, `I8`, `I16`, `I32`, `I64` | true |
| `kind` | **String** representing the kind of type | `integer` | true |
| `size` | **Number** of bits supported by the data type  | `8`, `16`, `32`, `64` | true |
| `signed` | **Boolean** indicating whether the integer is signed or unsigned | **Boolean** | true |

### Unsigned Integer Types
- U8
- U16
- U32
- U64

Example: Type Descriptor for `U64`
```json
{
    "name": "U64",
    "kind": "integer",
    "size": 64,
    "signed": false,
}
```

### Signed Integer Types
- I8
- I16
- I32
- I64

Example: Type Descriptor for `I8`
```json
{
    "name": "I8",
    "kind": "integer",
    "size": 8,
    "signed": true,
}
```

## Floating-Point Type Descriptors

| Field | Description | Options | Required |
| ----- | ----------- | ------- | -------- |
| `name` | **String** representing the FPP type name |  `F32`, `F64` | true |
| `kind` | **String** representing the kind of type | `float` | true |
| `size` | **Number** of bits supported by the data type  | `32`, `64` | true |

### Floating-Point Types
- F32
- F64

Example: Type Descriptor for F64
```json
{
    "name": "F64",
    "kind": "float",
    "size": 64,
}
```

## Boolean Type Descriptors

| Field | Description | Options | Required |
| ----- | ----------- | ------- | -------- |
| `name` | **String** representing the FPP type name | `bool` | true
| `kind` | **String** representing the kind of type | `bool` | true |
| `size` | **Number** of bits supported by the data type  | `8` | true |

### Boolean Types
- true
- false

Example: Type Descriptor for booleans
```json
{
    "name": "bool",
    "kind": "bool",
    "size": 8
}
```

## String Type Descriptors

| Field | Description | Options | Required |
| ----- | ----------- | ------- | -------- |
| `name` | **String** representing the FPP type name |  `string` | true |
| `kind` | **String** representing the kind of type | `string` | true | 
| `size` | **Number** representing the maximum string size in bytes | **Number** in the range [1, 2<sup>31</sup>) | true |

### String Types
Any sequence of characters

Example Type Descriptor for string
```json
{
    "name": "string",
    "kind": "string",
    "size": 80,
}
```

## Qualified Identifier Type Descriptors

A _Qualified Identifier_ is a kind of _[Type Descriptor](#type-descriptors)_ that refers to a _[Type Definition](#type-definitions)_.

| Field | Description | Options | Required |
| ----- | ----------- | ------- | -------- |
| `name` | **String** representing the fully qualified FPP type name |  Period-separated **String** | true |
| `kind` | **String** representing the kind of type | qualifiedIdentifier | true |


Example JSON of qualified name
```json
{
    "name": "Module1.MyArray",
    "kind": "qualifiedIdentifier",
}
```

# Type Definitions

## Array Type Definition

| Field | Description | Options | Required | 
| ----- | ----------- | ------- | -------- |
| `kind` | The kind of type | `array` | true |
| `qualifiedName` | Fully qualified name of element in FPP model | Period-separated **String** | true |
| `size` | Size of the data structure | **Number** | true |
| `elementType` | The type of the array's elements | **[Type Descriptor](#type-descriptors)** | true
| `default` | Default array value | Value of type specified in `elementType` | true |
| `annotation` | User-defined annotation | **String** | false |

Example FPP model with JSON representation:
```
module M1 {
    @ My array named A
    array A = [3] U8
}
```

```json
{
    "kind": "array",
    "qualifiedName": "M1.A",
    "size": 3,
    "elementType": {
        "name": "U8",
        "kind": "integer",
        "signed": false,
        "size": 8
    },
    "default": [0, 0, 0],
    "annotation": "My array named A"
}
```


## Enumeration Type Definition

| Field | Description | Options | Required |
| ----- | ----------- | ------- | -------- |
| `kind` | The kind of type | `enum` | true |
| `qualifiedName` | Fully qualified name of element in FPP model | Period-separated **String** | true |
| `representationType` | Type of the enumerated values | **[Type Descriptor](#type-descriptors)** | true |
| `enumeratedConstants` | The enumerated constants | JSON Dictionary of enumerated constants (keys) to [Enumerated Constant Descriptor](#enumerated-constant-descriptors) (values) | true |
| `default` | Qualified name of the enumeration's default value | **String** | true |
| `annotation` | User-defined annotation | **String** | false |

#### Enumerated Constant Descriptors

| Field | Description | Options | Required |
| ----- | ----------- | ------- | -------- |
| `name` | Name of the enumerated constant | **String** | true |
| `value` | Value associated to the enumerated constant | **Number** | true |
| `annotation` | User-defined annotation for the enumerated constant | **String** | false |

Example FPP model with JSON representation:
```
module M1 {
    @ Schroedinger's status
    enum Status {
        YES
        NO
        MAYBE @< The cat would know
    } default MAYBE
}
```
```json
{
    "kind": "enum",
    "qualifiedName": "M1.Status",
    "representationType": {
        "name": "I32",
        "kind": "integer",
        "signed": true,
        "size": 32
    },
    "enumeratedConstants": [
        {
            "name": "YES",
            "value" : 0
        },
        {
            "name": "NO",
            "value" : 1
        },
        {
            "name" : "MAYBE",
            "value" : 2,
            "annotation": "The cat would know"
        },
    ],
    "default": "M.Status.MAYBE",
    "annotation": "Schroedinger's status"
}
```

## Struct Type Definition

### Struct Type Definition

| Field | Description | Options | Required |
| ----- | ----------- | ------- | -------- |
| `kind` | The kind of type | `struct` | true |
| `qualifiedName` | Fully qualified name of element in FPP model | Period-separated **String** | true |
| `members` | The members of the struct | JSON dictionary of Member Name (key) to [Struct Member Descriptor](#struct-member-descriptor) (value) | true |
| `default` | The default value of the struct | JSON dictionary of Member Name (key) to default value (value) | true |
| `annotation` | User-defined annotation | **String** extracted from FPP model | false |

### Struct Member Descriptor

| Field | Description | Options | Required |
| ----- | ----------- | ------- | -------- |
| `type` | [Type Descriptor](#type-descriptors) of member | [Type Descriptor](#type-descriptors) | true |
| `index` | Index of the struct member | **Number** | true |
| `size` | Size of the struct member | **Number** | false |
| `format` | Format specifier | **String** | false |
| `annotation` | User-defined annotation | **String** extracted from FPP model | false |

Example FPP model with JSON representation:
```
module M1 {
    @ Struct for wxy values
    struct S {
        w: [3] U32 @< This is an array
        x: U32
        y: F32
    }
}
```
```json
{
    "kind": "struct",
    "qualifiedName": "M1.S",
    "annotation": "Struct for wxy values",
    "members": {
         "w": {
            "type": {
                "name": "M.S.w",
                "kind": "qualifiedIdentifier"
            },
            "index": 0,
            "size": 3,
            "annotation" : "This is an array"
        },
        "x": {
            "type": {
                "name": "U32",
                "kind": "integer",
                "signed": false,
                "size": 32
            },
            "format": "the count is {}",
            "index": 1
        },
        "y": {
            "type": {
                "name": "F32",
                "kind": "float",
                "size": 32
            },
            "index": 2
        }
    },
    "default": {
        "w": [0, 0, 0],
        "x": 0,
        "y": 0
    },
}
```

# Values

## Primitive Integer Values
**Number** representing integer value

Example JSON of type U8 with a value of 2:
```json
2
```

Example JSON of type I8 with a value of -2:
```json
-2
```

## Floating-Point Values
**Number** representing float value

Example JSON of type F32 with a value of 10.0
```json
10.5
```

## Boolean Values
**Boolean** value

Example JSON of type bool with a value of true

```json
true
```

## String Values
**String** containing sequence of characters

Example JSON of type string with a value of "Hello World!"
```json
"Hello World!"
```


## Array Values
**Array** with elements

Example JSON of an array of type U32 consisting of 10 elements
```json
[0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
```

## Enumeration Values
String qualified identifier name of enumeration value

Example JSON of an enum
```json
"Status.YES"
```

## Struct Values
**JSON Dictionary** consisting of String qualified identifier names (keys) and values (values)

Example JSON of a struct:
```json
{
    "S.w": [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
    "S.x": 20,
    "S.y": "Hello World!",
    "S.z": 15.5
}
```

## Invalid Values
### Null Values

| Field | Description | Options | Required |
| ----- | ----------- | ------- | -------- |
| `name` | **String** indicating that the value is null | null | true |
| `kind` | **String** indicating that the kind of value is invalid | invalid | true |

### Infinity Values

| Field | Description | Options | Required |
| ----- | ----------- | ------- | -------- |
| `name` | **String** indicating that the value is infinity|  infinity | true |
| `kind` | **String** indicating that the kind of value is invalid | invalid | true |

### Negative Infinity Values

| Field | Description | Options | Required |
| ----- | ----------- | ------- | -------- |
| `name` | **String** indicating that the value is negative infinity |  negativeInfinity | true |
| `kind` | **String** indicating that the kind of value is invalid | invalid | true |


# Commands, Telemetry Channels, Events, and Parameters

## Formal Parameters

Formal Parameters are used in Commands and Events definitions.

| Field | Description | Options | Required |
| ----- | ----------- | ------- | -------- |
| `name` | Name of parameter | **String** | true |
| `type` | [Type Descriptor](#type-descriptors) of parameter | [Type Descriptor](#type-descriptors) | true |
| `ref` | **Boolean** indicating whether the formal parameter is to be passed by referenced when it is used in a synchronous port invocation | **Boolean** | true |
| `annotation` | User-defined annotation of parameter | **String** | false |

```json
{
    "name": "param1",
    "type": {
        "name": "U32",
        "kind": "integer",
        "size": 32,
        "signed": false,
    },
    "ref": false,
    "annotation": "This is param1"
}
```

## Commands

| Field | Description | Options | Required |
| ----- | ----------- | ------- | -------- |
| `name` | Fully qualified name of the command | Period-separated **String** | true |
| `commandKind` | The kind of command | `async`, `guarded`, `sync`, `set`, `save` | true |
| `opcode` | Command opcode | **Number** | true |
| `formalParams` | Parameters of the command | Array of [Formal Parameters](#formal-parameters) | true |
| `annotation` | User-defined annotation of command | **String** | false |
| `priority` | Priority for the command on the input queue | **Number** | required for async command kinds |
| `queueFullBehavior` | Behavior of the command when the input full is queue | `assert`, `block`, `drop` | required for async command kinds |

Example Command in FPP:
```
@ A sync command with parameters
sync command SyncParams(
    param1: U32 @< Param 1
    param2: string @< Param 2
) opcode 0x100
```
JSON representation:
```json
{
    "name": "M1.SyncParams",
    "commandKind": "sync",
    "opcode": 256,
    "annotation": "A sync command with parameters",
    "formalParams": [
        {
            "name": "param1",
            "annotation": "Param 1",
            "type": {
                "name": "U32",
                "kind": "integer",
                "size": 32,
                "signed": false,
            },
            "ref": false
        },
         {
            "name": "param2",
            "annotation": "Param 2",
            "type": {
                "name": "string",
                "kind": "string",
                "size": "80"
            },
            "ref": false
        }
    ],
}
```

## Telemetry Channels

| Field | Description | Options | Required |
| ----- | ----------- | ------- | -------- |
| `name` | Fully qualified name of the telemetry channel | Period-separated **String** | true |
| `type` | [Type Descriptor](#type-descriptors) of the telemetry channel | [Type Descriptor](#type-descriptors) | true |
| `id` | Numeric identifier of the channel | **Number** | true |
| `telemetryUpdate` | Update specifier of the telemetry channel | `always`, `on change` | true |
| `format` | Format string of the channel | **String** | false |
| `annotation` | User-defined annotation of channel | **String** | false |
| `limit` | Low and high limits of the channel | **JSON dictionary** | false |

Example FPP model with JSON representation:
```
@ Telemetry channel 1
telemetry Channel1: F64 \
    id 0x100 \
    update on change \
    low { yellow -1, orange -2, red -3 } \
    high { yellow 1, orange 2, red 3 }
```

```json
[
    {
        "name": "M1.Channel1",
        "annotation": "Telemetry channel 1",
        "type": {
            "name": "F64",
            "kind": "float",
            "size": 64
        },
        "id": 256,
        "telemetryUpdate": "on change",
        "limit": {
            "low": {
                "yellow": -1,
                "orange": -2,
                "red": -3
            },
            "high": {
                "yellow": 1,
                "orange": 2,
                "red": 3
            }
        }
    }
]
```

## Events

| Field | Description | Options | Required |
| ----- | ----------- | ------- | -------- |
| `name` | Fully qualified name of the event | Period-separated **String** | true |
| `severity` | Severity of the event | `ACTIVITY_HI`, `ACTIVITY_LO`, `COMMAND`, `DIAGNOSTIC`, `FATAL`, `WARNING_HI`, `WARNING_LO` | true |
| `formalParams` | Parameters of the event | Array of [Formal Parameters](#formal-parameters) | true |
| `id` | Numeric identifier of the event | **Number** | true |
| `format` | Format string of the event | **String** | true |
| `throttle` | Maximum number of times to emit the event before throttling it | **Number** | false |
| `annotation` | User-defined annotation of the event | **String** | false |


Example FPP model with JSON representation:
```
@ This is the annotation for Event 0
event Event0 \
    severity activity low \
    id 0x100 \
    format "Event 0 occurred"
```

```json
{
    "name": "M1.Event0",
    "annotation": "This is the annotation for Event 0",
    "severity": "ACTIVITY_LO",
    "formalParams": [],
    "id": 256,
    "format": "Event 0 occurred",
}
```

Example FPP model with JSON representation:
```
@ This is the annotation for Event 1
@ Sample output: "Event 1 occurred with argument 42"
event Event1(
    arg1: U32 @< Argument 1
) \
    severity activity high \
    id 0x101 \
    format "Event 1 occurred with argument {}"

```
```json
{
    "name": "M1.Event1",
    "annotation": "This is the annotation for Event 1\nSample output: \"Event 1 occurred with argument 42\"",
    "severity": "ACTIVITY_HIGH",
    "formalParams": [
        {
           "name": "arg1",
            "annotation": "Argument 1",
            "type": {
                "name": "U32",
                "kind": "integer",
                "size": 32,
                "signed": false,
            },
            "ref": false  
        }
    ],
    "id": 257,
    "format": "Event 1 occurred with argument {}",
}
```

## Parameters

| Field | Description | Options | Required |
| ----- | ----------- | ------- | -------- |
| `name` | Fully qualified name of the parameter | Period-separated **String** | true |
| `type` | [Type Descriptor](#type-descriptors) of the parameter | [Type Descriptor](#type-descriptors) | true |
| `id` | Numeric identifier of the parameter | **Number** | true |
| `default` | Default value (of type specified in `type`)  of the parameter | Value of type specified in `type` | false |
| `annotation` | User-defined annotation of parameter | **String** | false |

Example FPP model with JSON representation:
```
@ This is the annotation for Parameter 1
param Parameter1: U32 \
    id 0x100 \
    set opcode 0x101 \
    save opcode 0x102
```

```json
{
    "name": "M1.Parameter1",
    "type": {
        "name": "U32",
        "kind": "integer",
        "signed": false,
        "size": 32
    },
    "id": "256",
    "annotation": "This is the annotation for Parameter 1",
    "default": 0
}
```

# Data Products
## Records

| Field | Description | Options | Required |
| ----- | ----------- | ------- | -------- |
| `name` | Fully qualified name of the record | Period-separated **String** | true |
| `type` | [Type Descriptor](#type-descriptors) the record | [Type Descriptor](#type-descriptors) | true |
| `array` | **Boolean** specifying whether the record stores a variable number of elements | **Boolean** | true |
| `id` | The numeric identifier of the record | **Number** | true |
| `annotation` | User-defined annotation of record | **String** | false |

Example FPP model with JSON representation:
```
@ Record 0: A variable number of F32 values
@ Implied id is 0x100
product record Record0: F32 array

@ Record 1: A single U32 value
product record Record1: U32 id 0x102
```

```json
[
    {
        "name": "M1.Record0",
        "annotation": "Record 0: A variable number of F32 values\nImplied id is 0x100",
        "type": {
            "name": "F32",
            "kind": "float",
            "size": 32
        },
        "array": true,
        "id": 256 
    },
    {
        "name": "M1.Record1",
        "annotation": "Record 1: A single U32 value",
        "type": {
            "name": "U32",
            "kind": "integer",
            "signed": false,
            "size": 32
        },
        "array": false,
        "id": 258
    }      
]
```

## Containers

| Field | Description | Options | Required |
| ----- | ----------- | ------- | -------- |
| `name` | Fully qualified name of the container | Period-separated **String** | true |
| `id` | The numeric identifier of the record | **Number** | true |
| `defaultPriority` | The downlink priority for the container | **Number** | false |
| `annotation` | User-defined annotation of container | **String** | false |

Example FPP model with JSON representation:
```
@ Container 0
@ Implied id is 0x100
product container Container0

@ Container 1
product container Container1 id 0x102

@ Container 2
@ Implied id is 0x103
product container Container2 default priority 10
```

```json
[
    {
       "name": "M1.Container0",
       "annotation": "Container 0\nImplied id is 0x100",
       "id": 256,
    },
    {
        "name": "M1.Container1",
        "annotation": "Container 1",
        "id": 258,
    },
    {
        "name": "M1.Container2",
        "annotation": "Container 2\nImplied id is 0x103",
        "id": 3,
        "defaultPriority": 259
    }
]
```

# Dictionaries
## Dictionary Metadata

| Field | Description | Options | Required |
| ----- | ----------- | ------- | -------- |
| `deploymentName` | **String** representing the deployment name | **String** | true |
| `frameworkVersion` | **String** representing the F´ framework version (semantic versioning) | **String** | true |
| `projectVersion` | **String** representing the project version (semantic versioning) | **String** | true |
| `libraryVersions` | **Array of Strings** corresponding to the version (semantic versioning) of libraries used by the F´ project | **Array of Strings** | true
| `dictionarySpecVersion` | **String** representing the JSON dictionary specification version | **String** | true |
```json
{
    "deploymentName": "MyDeployment",
    "frameworkVersion": "3.3.2",
    "projectVersion": "1.0.0",
    "libraryVersions": [],
    "dictionarySpecVersion": "1.0.0"
}
```

## Dictionary Content

| Field | Content | Required |
| ----- | ------- | -------- |
| `metadata` | [Dictionary Metadata](#dictionary-metadata) | true |
| `typeDefinitions` | Array of [Type Definitions](#type-definitions)| true |
| `commands` | Array of [Commands](#commands) | true |
| `events` | Array of [Events](#events) | true |
| `telemetryChannels` | Array of [Telemetry Channels](#telemetry-channels) | true |
| `parameters` | Array of [Parameters](#parameters) | true |
| `records` | Array of [Records](#records) | true |
| `containers` | Array of [Containers](#containers) | true |

```json
{
  "metadata": {
    "deploymentName": "MyDeployment",
    "frameworkVersion": "3.3.2",
    "projectVersion": "1.0.0",
    "libraryVersions": [],
    "dictionarySpecVersion": "1.0.0"
  },
  "typeDefinitions" : [
    {
      "kind" : "array",
      "qualifiedName" : "FppTest.StringArray",
      "size" : 2,
      "elementType" : {
        "name" : "string",
        "kind" : "string",
        "size" : 80
      },
      "default" : [
        "A",
        "B"
      ],
      "annotation" : "An array of 2 String values"
    },
    {
      "kind" : "enum",
      "qualifiedName" : "FppTest.MyEnum",
      "representationType" : {
        "name" : "U8",
        "kind" : "integer",
        "size" : 8,
        "signed" : false
      },
      "enumeratedConstants" : [
        {
          "name" : "YES",
          "value" : 0
        },
        {
          "name" : "NO",
          "value" : 1
        },
        {
          "name" : "MAYBE",
          "value" : 2,
          "annotation" : "MAYBE is a maybe"
        }
      ],
      "default" : "FppTest.MyEnum.YES"
    },
    {
      "kind" : "struct",
      "qualifiedName" : "FppTest.MyStruct",
      "members" : {
        "x" : {
          "type" : {
            "name" : "U64",
            "kind" : "integer",
            "size" : 64,
            "signed" : false
          },
          "index" : 0,
          "format" : "The value of x is {}"
        },
        "y" : {
          "type" : {
            "name" : "F32",
            "kind" : "float",
            "size" : 32
          },
          "index" : 1,
          "format" : "The value of y is {.2f}",
          "annotation" : "This is the y parameter"
        }
      },
      "default" : {
        "x" : 1,
        "y" : 1.5
      }
    }
  ],
  "commands" : [
    {
      "name" : "c1.CommandString",
      "commandKind" : "sync",
      "opcode" : 5000,
      "formalParams" : [
        {
          "name" : "a",
          "type" : {
            "name" : "FppTest.StringArray",
            "kind" : "qualifiedIdentifier"
          },
          "ref" : false,
          "annotation" : "description for argument a"
        },
        {
          "name" : "b",
          "type" : {
            "name" : "U32",
            "kind" : "integer",
            "size" : 32,
            "signed" : false
          },
          "ref" : false,
          "annotation" : "description for argument b"
        }
      ],
      "annotation" : "Command with 2 args (array of strings and U32)"
    },
    {
      "name" : "c1.PARAM1_PARAM_SET",
      "commandKind" : "set",
      "opcode" : 5001,
      "formalParams" : [
        {
          "name" : "val",
          "type" : {
            "name" : "FppTest.MyStruct",
            "kind" : "qualifiedIdentifier"
          },
          "ref" : false
        }
      ],
      "annotation" : "Parameter (struct)"
    },
    {
      "name" : "c1.PARAM1_PARAM_SAVE",
      "commandKind" : "save",
      "opcode" : 5002,
      "formalParams" : [
      ],
      "annotation" : "Parameter (struct)"
    }
  ],
  "parameters" : [
    {
      "name" : "c1.Param1",
      "type" : {
        "name" : "FppTest.MyStruct",
        "kind" : "qualifiedIdentifier"
      },
      "id" : 5001,
      "default" : {
        "x" : 2,
        "y" : 1.5
      },
      "annotation" : "Parameter (struct)"
    }
  ],
  "events" : [
    {
      "name" : "c1.Event1",
      "severity" : "ACTIVITY_HI",
      "formalParams" : [
        {
          "name" : "arg1",
          "type" : {
            "name" : "FppTest.MyEnum",
            "kind" : "qualifiedIdentifier"
          },
          "ref" : false,
          "annotation" : "description of arg1 formal param"
        }
      ],
      "id" : 5000,
      "format" : "Arg one is {} and there is no arg 2",
      "annotation" : "Event with array arg (containing 4 F32 values)"
    }
  ],
  "telemetryChannels" : [
    {
      "name" : "c1.MyTlmChannel2",
      "type" : {
        "name" : "F32",
        "kind" : "float",
        "size" : 32
      },
      "id" : 5000,
      "telemetryUpdate" : "always",
      "annotation" : "Telemetry channel of type F32"
    }
  ],
  "records" : [
    {
      "name" : "c1.U32Record",
      "type" : {
        "name" : "U32",
        "kind" : "integer",
        "size" : 32,
        "signed" : false
      },
      "array" : false,
      "id" : 5100,
      "annotation" : "Record 1"
    }
  ],
  "containers" : [
    {
      "name" : "c1.Container1",
      "id" : 5100,
      "defaultPriority" : 10,
      "annotation" : "Container 1"
    }
  ]
}

```
