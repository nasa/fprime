# FPP JSON Dictionary Specification
This document describes the format of FPP JSON dictionaries.

## Contents
- [Type Names](#type-names)
  - [Primitive Integer Type Names](#primitive-integer-type-names)
    - [Unsigned Integer Types](#unsigned-integer-types)
    - [Signed Integer Types](#signed-integer-types)
  - [Floating-Point Type Names](#floating-point-type-names)
    - [Floating-Point Types](#floating-point-types)
  - [Boolean Type Name](#boolean-type-name)
    - [Boolean Types](#boolean-types)
  - [String Type Names](#string-type-names)
    - [String Types](#string-types)
  - [Qualified Identifier Type Names](#qualified-identifier-type-names)
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
- [Parameters, Commands, Telemetry Channels, and Events](#parameters-commands-telemetry-channels-and-events)
  - [Formal Parameters](#formal-parameters)
  - [Parameters](#parameters)
  - [Commands](#commands)
  - [Telemetry Channels](#telemetry-channels)
  - [Events](#events)
- [Data Products](#data-products)
  - [Records](#records)
  - [Containers](#containers)
- [Dictionaries](#dictionaries)
  - [Dictionary Metadata](#dictionary-metadata)
  - [Dictionary Content](#dictionary-content)

# Type Names

## Primitive Integer Type Names
| Field | Description | Options | Required | 
| ----- | ----------- | ------- | -------- |
| `name` | **String** representing the FPP type name |  U8, U16, U32, U64, I8, I16, I32, I64 | true |
| `kind` | **String** representing the kind of type | integer | true |
| `size` | **Number** of bits supported by the data type  | 8, 16, 32, 64 | true |
| `signed` | **Boolean** indicating whether the integer is signed or unsigned | **Boolean** | true |

### Unsigned Integer Types
- U8
- U16
- U32
- U64

Example JSON of U8
```json
{
    "name": "U8",
    "kind": "integer",
    "size": 8,
    "signed": false,
}
```


Example JSON of U64
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

Example JSON of I8
```json
{
    "name": "I8",
    "kind": "integer",
    "size": 8,
    "signed": true,
}
```


Example JSON of I64
```json
{
    "name": "I64",
    "kind": "integer",
    "size": 64,
    "signed": true,
}
```

## Floating-Point Type Names

| Field | Description | Options | Required |
| ----- | ----------- | ------- | -------- |
| `name` | **String** representing the FPP type name |  F32, F64 | true |
| `kind` | **String** representing the kind of type | float | true |
| `size` | **Number** of bits supported by the data type  | 32, 64 | true |

### Floating-Point Types
- F32
- F64

Example JSON of F32
```json
{
    "name": "F32",
    "kind": "float",
    "size": 32,
}
```

Example JSON of F64
```json
{
    "name": "F64",
    "kind": "float",
    "size": 64,
}
```

## Boolean Type Name

| Field | Description | Options | Required |
| ----- | ----------- | ------- | -------- |
| `name` | **String** representing the FPP type name | bool | true
| `kind` | **String** representing the kind of type | bool | true |
| `size` | **Number** of bits supported by the data type  | 8 | true |

### Boolean Types
- true
- false

Example JSON of bool
```json
{
    "name": "bool",
    "kind": "bool",
    "size": 8
}
```

## String Type Names
| Field | Description | Options | Required |
| ----- | ----------- | ------- | -------- |
| `name` | **String** representing the FPP type name |  string | true |
| `kind` | **String** representing the kind of type | string | true | 
| `size` | **Number** of bytes supported by the data type | **Number** in the range [0, 2<sup>31</sup>) | false |

### String Types
Any sequence of characters

Example JSON of string
```json
{
    "name": "string",
    "kind": "string",
    "size": 64,
}
```

## Qualified Identifier Type Names
| Field | Description | Options | Required |
| ----- | ----------- | ------- | -------- |
| `name` | **String** representing the FPP type name |  Period separated **String** | true |
| `kind` | **String** representing the kind of type | qualifiedIdentifier | true |


Example JSON of qualified name
```json
{
    "name": "M.a",
    "kind": "qualifiedIdentifier",
}
```

# Type Definitions

## Array Type Definition
| Field | Description | Options | Required | 
| ----- | ----------- | ------- | -------- |
| `kind` | **String** representing the kind of type | array | true |
| `qualifiedName` | **String** representing unique qualified name of element in FPP model | Period separated **String** | true |
| `size` | Max **Number** of elements that can be in the data structure | **Number** | true |
| `elementType` | A **JSON dictionary** representing the type of array | **JSON Dictionary** | true
| `default` | Default array value | Value of type specified in `elementType` | true |


Example FPP model with JSON representation:
```
module M {
  array A = [3] U8
}
```

```json
{
    "kind": "array",
    "qualifiedName": "M.A",
    "size": 3,
    "elementType": {
        "name": "U8",
        "kind": "integer",
        "signed": false,
        "size": 8
    },
    "default": [0, 0, 0]
}
```



## Enumeration Type Definition
| Field | Description | Options | Required |
| ----- | ----------- | ------- | -------- |
| `kind` | String representing the kind of type | enum | true |
| `qualifiedName` | String representing unique qualified name of element in FPP model | Period separated **String** | true |
| `representationType` | The [Type Name](#type-names) of values in the enumeration | [Type Name](#type-names) | true |
| `identifiers` | Dictionary of identifiers (keys) and numeric values (values) | **JSON Dictionary** | true |
| `default` | String qualified name of the enumeration value | **String** qualified name | true |

Example FPP model with JSON representation:
```
module M {
    enum Status {
        YES
        NO
        MAYBE
    } default MAYBE
}
```
```json
{
    "kind": "enum",
    "qualifiedName": "M.Status",
    "representationType": {
        "name": "I32",
        "kind": "integer",
        "signed": true,
        "size": 32
    },    
    "identifiers": {
        "YES": 0, 
        "NO": 1,
        "MAYBE": 2
    },
    "default": "M.Status.MAYBE"
}
```

## Struct Type Definition

### Struct Member
| Field | Description | Options | Required |
| ----- | ----------- | ------- | -------- |
| `type` | [Type Name](#type-names) of member | [Type Name](#type-names) | true |
| `index` | **Number** index of the struct member | **Number** | true |
| `size` | **Number** representing the size of the struct member | **Number** | false |
| `format` | **String** format specifier | **String** | false |

### Struct Type Definition
| Field | Description | Options | Required |
| ----- | ----------- | ------- | -------- |
| `kind` | String representing the kind of type | struct | true |
| `qualifiedName` | String representing unique qualified name of element in FPP model | Period separated **String** | true |
| `members` | JSON dictionary consisting of **String** identifier (key) and [Struct Member](#struct-member) (value) | JSON dictionary | true |
| `default` | JSON dictionary consisting of **String** identifier (key) and default value (value) | JSON dictionary | true |

Example FPP model with JSON representation:
```
module M {
    struct A {
        w: [3] U32
        x: U32
        y: F32
    }
}
```
```json
{
    "kind": "struct",
    "qualifiedName": "M.A",
    "members": {
         "w": {
            "type": {
                "name": "M.A.w",
                "kind": "qualifiedIdentifier"
            },
            "index": 0,
            "size": 3
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


# Parameters, Commands, Telemetry Channels, and Events

## Formal Parameters
| Field | Description | Options | Required |
| ----- | ----------- | ------- | -------- |
| `name` | **String** name | **String** | true |
| `description` | **String** annotation of parameter | **String** | true |
| `type` | [Type Name](#type-names) of parameter | [Type Name](#type-names) | true |
| `ref` | **Boolean** indicating whether the formal parameter is to be passed by referenced when it is used in a synchronous port invocation | **Boolean** | true |

```json
{
    "name": "param1",
    "description": "Param 1",
    "type": {
        "name": "U32",
        "kind": "integer",
        "size": 32,
        "signed": false,
    },
    "ref": false
}
```

## Parameters
| Field | Description | Options | Required |
| ----- | ----------- | ------- | -------- |
| `name` | **String** qualified name of the parameter | Period separated **String** | true |
| `description` | **String** annotation of parameter | **String** | true |
| `type` | [Type Name](#type-names) of the parameter | [Type Name](#type-names) | true |
| `default` | Default value (of type specified in `type`)  of the parameter | Value of type specified in `type` | false |
| `id` | **Number** representing the numeric identifier of the parameter | **Number** | true |

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
    "name": "c1.Parameter1",
    "description": "This is the annotation for Parameter 1",
    "type": {
        "name": "U32",
        "kind": "integer",
        "signed": false,
        "size": 32
    },
    "default": 0,
    "id": "256"
}
```

## Commands
| Field | Description | Options | Required |
| ----- | ----------- | ------- | -------- |
| `name` | **String** qualified name of the command | Period separated **String** | true |
| `commandKind` | **String** representing the kind of command | async, guarded, sync | true |
| `opcode` | **Number** command opcode | **Number** | true |
| `description` | **String** annotation of command | string | true |
| `formalParams` | Array of [Formal Parameters](#formal-parameters) | Array of [Formal Parameters](#formal-parameters) | true |
| `priority` | **Number** representing the priority for the command on the input queue | **Number** | required for async command kinds |
| `queueFullBehavior` | **String** representing the behavior of the command when the input full is queue | assert, block, drop | required for async command kinds |

Example FPP model with JSON representation:
```
@ A sync command with parameters
sync command SyncParams(
    param1: U32 @< Param 1
    param2: string @< Param 2
) opcode 0x100
```

```json
{
    "name": "c1.SyncParams",
    "commandKind": "sync",
    "opcode": 256,
    "description": "A sync command with parameters",
    "formalParams": [
        {
            "name": "param1",
            "description": "Param 1",
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
            "description": "Param 2",
            "type": {
                "name": "string",
                "kind": "string",
                "size": ""
            },
            "ref": false
        }
    ],
}
```

## Telemetry Channels
| Field | Description | Options | Required |
| ----- | ----------- | ------- | -------- |
| `name` | **String** qualified name of the telemetry channel | Period separated **String** | true |
| `description` | **String** annotation of channel | **String** | true |
| `type` | [Type Name](#type-names) the telemetry channel | [Type Name](#type-names) | true |
| `id` | **Number** representing numeric identifier | **Number** | true |
| `telemetryUpdate` | **String** representing when the telemetry channel can update | always, on change | true |
| `format` | **String** format with a single argument (the telemetry channel) | **String** | false |
| `limit` | **JSON dictionary** consisting of low and high limits | **JSON dictionary** | false |

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
        "name": "c1.Channel1",
        "description": "Telemetry channel 1",
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
| `name` | **String** qualified name of the event | Period separated **String** | true |
| `description` | **String** annotation of event | **String** | true |
| `severity` | **String** representing severity of the event | ACTIVITY_HIGH, ACTIVITY_LOW, COMMAND, DIAGNOSTIC, FATAL, WARNING_HIGH, WARNING_LOW | true |
| `formalParams` | Array of [Formal Parameters](#formal-parameters) | Array [Formal Parameters](#formal-parameters) | true |
| `id` | **Number** representing the numeric identifier of the event | **Number** | true |
| `format` | **String** format with event parameters as arguments | **String** | false |
| `throttle` | **Number** representing the maximum number of times to emit the event before throttling it | **Number** | false |


Example FPP model with JSON representation:
```
@ This is the annotation for Event 0
event Event0 \
    severity activity low \
    id 0x100 \
    format "Event 0 occurred"

...
```

```json
{
    "name": "c1.Event0",
    "description": "This is the annotation for Event 0",
    "severity": "ACTIVITY_LOW",
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
    "name": "c1.Event1",
    "description": "This is the annotation for Event 1",
    "severity": "ACTIVITY_HIGH",
    "formalParams": [
        {
           "name": "arg1",
            "description": "Argument 1",
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

# Data Products
## Records
| Field | Description | Options | Required |
| ----- | ----------- | ------- | -------- |
| `name` | **String** qualified name of the record | Period separated **String** | true |
| `description` | **String** annotation of record | **String** | true |
| `type` | [Type Name](#type-names) the record | [Type Name](#type-names) | true |
| `array` | **Boolean** specifying whether the record stores a variable number of elements | **Boolean** | false |
| `id` | **Number** representing the numeric identifier of the record | **Number** | true |

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
        "name": "c1.Record0",
        "description": "Record 0: A variable number of F32 values",
        "type": {
            "name": "F32",
            "kind": "float",
            "size": 32
        },
        "array": true,
        "id": 256 
    },
    {
        "name": "c1.Record1",
        "description": "Record 1: A single U32 value",
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
| `name` | **String** qualified name of the container | Period separated **String** | true |
| `description` | **String** annotation of container | **String** | true |
| `id` | **Number** representing the numeric identifier of the record | **Number** | true |
| `defaultPriority` | **Number** representing the downlink priority for the container | **Number** | false |

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
       "name": "c1.Container0",
       "description": "Container 0\nImplied id is 0x100",
       "id": 256,
    },
    {
        "name": "c1.Container1",
        "description": "Container 1",
        "id": 258,
    },
    {
        "name": "c1.Container2",
        "description": "Container 2\nImplied id is 0x103",
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
| Field | Description | Options | Required |
| ----- | ----------- | ------- | -------- |
| `metadata` | [Dictionary Metadata](#dictionary-metadata) | [Dictionary Metadata](#dictionary-metadata) | true |
| `typeDefinitions` | Array of [Arrays](#array-type-definition), [Enums](#enumeration-type-definition), and [Structs](#struct-type-definition-1) | Array of [Arrays](#array-type-definition), [Enums](#enumeration-type-definition), and [Structs](#struct-type-definition-1) | true |
| `commands` | Array of [Commands](#commands) | Array of [Commands](#commands) | true |
| `events` | Array of [Events](#events) | Array of [Events](#events) | true |
| `telemetryChannels` | Array of [Telemetry Channels](#telemetry-channels) | Array of [Telemetry Channels](#telemetry-channels) | true |
| `parameters` | Array of [Parameters](#parameters) | Array of [Parameters](#parameters) | true |
| `records` | Array of [Records](#records) | Array of [Records](#records) | true |
| `containers` | Array of [Containers](#containers) | Array of [Containers](#containers) | true |

```json
{
    "metadata": {
        "deploymentName": "MyDeployment",
        "frameworkVersion": "3.3.2",
        "projectVersion": "1.0.0",
        "libraryVersions": [],
        "dictionarySpecVersion": "1.0.0"
    },
    "typeDefinitions": [
        {
            "kind": "enum",
            "qualifiedName": "M.C.Status",
            "representationType": {
                "name": "I32",
                "kind": "integer",
                "signed": true,
                "size": 32
            },    
            "identifiers": {
                "YES": 0, 
                "NO": 1,
                "MAYBE": 2
            },
            "default": "M.C.Status.MAYBE"
        },
        {
            "kind": "array",
            "qualifiedName": "M.C.A1",
            "size": 3,
            "elementType": {
                "name": "U8",
                "kind": "integer",
                "signed": false,
                "size": 8
            },
            "default": [0, 0, 0]
        },
        {
            "kind": "array",
            "qualifiedName": "M.C.A2",
            "size": 5,
            "elementType": {
                "name": "string",
                "kind": "string",
            },
            "default": ["a", "b", "c", "d", "e"]
        },
        {
            "kind": "struct",
            "qualifiedName": "M.S1",
            "members": {
                "w": {
                    "type": {
                        "name": "M.C.A1",
                        "kind": "qualifiedIdentifier"
                    },
                    "index": 0,
                    "size": 3
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
        },
        {
            "kind": "struct",
            "qualifiedName": "M.C.S2",
            "members": {
                "x": {
                    "type": {
                        "name": "string",
                        "kind": "string",
                    },
                    "format": "the string is {}",
                    "index": 0
                },
            },
            "default": {
                "w": "hello world!"
            },
        }
    ],
    "commands": [
        {
            "commandKind": "sync",
            "opcode": 257,
            "name": "c1.SyncParams",
            "description": "A sync command with parameters",
            "formalParams": [
                {
                    "name": "param1",
                    "description": "Param 1",
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
                    "description": "Param 2",
                    "type": {
                        "name": "string",
                        "kind": "string",
                        "size": ""
                    },
                    "ref": false
                }
            ],
        }
    ],
    "events": [
        {
            "name": "c1.Event0",
            "description": "This is the annotation for Event 0",
            "severity": "activity low",
            "formalParams": [
                {
                    "name": "",
                    "description": "",
                    "type": {},
                    "ref": false
                }
            ],
            "id": 256,
            "format": "Event 0 occurred",
            "throttle": ""
        },
        {
            "name": "c1.Event1",
            "description": "This is the annotation for Event 1",
            "severity": "activity high",
            "formalParams": [
                {
                "name": "arg1",
                    "description": "Argument 1",
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
            "throttle": ""
        }
    ],
    "telemetryChannels": [
        {
            "name": "c1.Channel1",
            "description": "Telemetry channel 1",
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
    ],
    "parameters": [
        {
            "name": "c1.Parameter1",
            "description": "This is the annotation for Parameter 1",
            "type": {
                "name": "U32",
                "kind": "integer",
                "signed": false,
                "size": 32
            },
            "default": 0,
            "id": 256
        }
    ],
    "records": [    
        {
            "name": "c1.Record1",
            "description": "Record 1: A single U32 value",
            "type": {
                "name": "U32",
                "kind": "integer",
                "signed": false,
                "size": 32
            },
            "array": false,
            "id": 258
        }      
    ],
    "containers": [
        {
            "name": "c1.Container0",
            "description": "Container 0\nImplied id is 0x100",
            "id": 256,
        }
    ]
}
```
