# F´ XML Specifications

**Note:** For a hands-on walk-through of build topologies, please see: [Tutorials](../../Tutorials/README.md). This is
an advanced specification and the hands-on tutorial is likely better for new users.

Serializable types, arrays, ports, and components are defined in XML files.
Those files are parsed by the code generator, and files containing C++
class declarations are created. This section will describe the syntax of
the XML files.

This guide describes the XML format for:

-[Serializable](#serializable)
-[Ports](#port)
-[Components](#components)
-[Topologies](#topologies)

## Serializable

Serializables are the types that are passed between components in the
architecture and are used for ground data services such as commands,
telemetry, events, and parameters (see the component description in
Section 6.6.3 for what these are). A serializable type is a complex or
basic type that can be converted to a data buffer that contains the
values of an instance of the type. This data buffer can be passed around
the system (and to ground software) in a generic way and reconstituted
into the original type as needed. Basic C/C++ types like int and float
are supported automatically in the architecture, but the user is also
allowed to define arbitrary complex types that can be serialized. This
is done in one of two ways, either by hand-coding a class or allowing
the code generator to generate the class. These types can then be used
in XML specifications of ports and components.

### XML-Specified Serializable

In most cases, a complex type is representable as a collection of simple
types. This is analogous to a C struct with data members. The code for
serializing and deserializing a struct is straightforward, so a code
generator is provided that will create the classes for a serializable
struct. The user specifies the members in XML, and the C++ class is
generated. The types of the members can be basic types or other
serializables, either XML or hand-coded. In order for the build system
to detect that a file contains the XML for a serializable type, the file
must follow the naming convention \<SomeName\>SerializableAi.xml. An
example of this can be found in
Autocoders/templates/ExampleSerializableAi.xml. Table 16 describes the
XML tags and attributes to use for the serializable.

**Table 16.** Serializable XML
specification.

| Tag                        | Attribute | Description                                                                                                                                          |
| -------------------------- | --------- | ---------------------------------------------------------------------------------------------------------------------------------------------------- |
| serializable               |           | The outermost tag that indicates a serializable is being defined.                                                                                    |
| serializable               | namespace | The C++ namespace for the serializable class (optional).                                                                                             |
| serializable               | name      | The class name for the serializable.                                                                                                                 |
| serializable               | typeid    | An integer uniquely identifying the type. Optional; generated from hash otherwise. Should be unique across the application.                          |
| comment                    |           | Used for a comment describing the type. Is placed as a Doxygen-compatible tag in the class declaration.                                              |
| members                    |           | Starts the region of the declaration where type members are specified.                                                                               |
| member                     |           | Defines a member of the type.                                                                                                                        |
| member                     | name      | Defines the member name.                                                                                                                             |
| member                     | type      | The type of the member. Should be a built-in type, ENUM, string, an XML-specified type, or a user-written serializable type. |
| member                     | size      | Required when type is string; otherwise not allowed. Specifies the string size.|
| member                     | array\_size | Specifies that the member is an array of the type with the specified size.|
| member                     | format    | Specifies a format specifier when displaying the member.                                                                                             |
| default                    |           | Specifies the default value of the member (optional).                                                                              |
| enum                       |           | Specifies an enumeration when the member type=ENUM.                                                                                                  |
| enum                       | name      | Enumeration type name.                                                                                                                               |
| item                       |           | Specifies a member of the enumeration.                                                                                                               |
| item                       | name      | Specifies the name of the enumeration member.                                                                                                        |
| item                       | value     | Assigns a value to the enumeration member. Member values in the enumeration follow C enumeration rules if not specified.                             |
| item                       | comment   | A comment about the member. Becomes a Doxygen tag.                                                                                                   |
| import\_serializable\_type |           | Imports an XML definition of another serializable type for use in the definition.                                                                    |
| import\_enum\_type         |           | Imports an XML definition of an enumeration type.|
| import\_array\_type        |           | Imports an XML definition of an array type.|
| include\_header            |           | Includes a C/C++ user-written header for member types.                                                                                               |

#### Constraints

##### Ground Interfaces

Serializables used for defining the interfaces for the ground system
must be fully specified in XML files. Members of the serializables
cannot be hand-coded types since the ground system analyzes the XML to
determine the types for displaying and archiving the data. (See Section
6.6.3 for component ground interface specifications.)

### XML-Specified Enumeration

#### Motivation

As discussed in the previous section, you can define an enumeration
when specifying the type of a member of a Serializable type.
For example, you can create a file `SwitchSerializableAi.xml` containing
this specification:

```xml
<serializable name="Switch">
  <members>
    <member name="state" type="ENUM">
      <enum name="SwitchState">
        <item name="OFF" value="0"/>
        <item name="ON" value="1"/>
      </enum>
      <default>OFF</default>
    </member>
  </members>
</serializable>
```

This file defines a Serializable type `Switch`
with one member `state`.
Its type is `SwitchState`, which is an enumeration with
enumerated constants `OFF` and `ON`. Using the default child element,
the default value will be OFF.

Alternatively, you can specify an enumeration *E* as a separate XML type.
Then you can do the following:

1. Generate a C++ representation of *E* that you can include in C++
files and use on its own.

2. Use the XML representation of *E* in Serializable XML types, Array XML
types, port arguments, telemetry channels, and event arguments.

As an example, you can create a file `SwitchStateEnumAi.xml` that specifies an
XML enumeration type `SwitchState`
with enumerated constants `OFF` and `ON` and set to `OFF` by default, like this:

```xml
<enum name="SwitchState" default="OFF">
  <item name="OFF" value="0"/>
  <item name="ON" value="1"/>
</enum>
```

By running the code generator on this file, you can generate C++
files `SwitchStateEnumAc.hpp` and `SwitchStateEnumAc.cpp`
that define the C++ representation of the type.
Anywhere that you include `SwitchStateEnumAc.hpp` in your C++ code, you can
use the enumerated constants `SwitchState::OFF` and `SwitchState::ON`.
If you import `SwitchStateEnumAi.xml` into another XML definition,
then you can use the type `SwitchState` there.

To use an XML enumeration type *E* in another XML definition *D*,
enclose the name of the file that defines *E* in an XML tag `import_enum_type`.
As an example, you can revise the `Switch` Serializable
definition shown above.
Instead of defining `SwitchState` as an inline enumeration, you can use the
`SwitchState` XML enumeration type as follows:

```xml
<serializable name="Switch">
  <import_enum_type>SwitchStateEnumAi.xml</import_enum_type>
  <members>
    <member name="state" type="SwitchState"/>
  </members>
</serializable>
```

Notice that the revised version (1) imports the enum type definition
from the file, `SwitchStateEnumAi.xml` and (2) uses the named
type `SwitchState` as the type of member `state`.

As another example, if you import the file `SwitchStateEnumAi.xml` into the
definition of a component *C*, then you can use the type `SwitchState` in the
telemetry dictionary for *C*.
When a value of type `SwitchState` is emitted as telemetry, the GDS
will display it symbolically as `OFF` or `ON`.

#### Specification

**File name:** An XML enumeration type *E* must
be defined in a file with the name *E* `EnumAi.xml`.
For example, the XML enumeration type `SwitchState`
must be defined in a file named `SwitchStateEnumAi.xml`.

**Top-level structure:** An XML enumeration type is an XML node named `enum`
with attributes *enum_attributes* and children *enum_children*.

`<enum` *enum_attributes* `>` *enum_children* `</enum>`

**Enum attributes:** *enum_attributes* consists of the following:

* An attribute `name` giving the name of the enumeration type.

* An optional attribute `namespace` giving the enclosing namespace
of the enumeration type.
The namespace consists of one or more identifiers separated by `::`.

* An optional attribute `default` giving the default value
of the enumeration. This value must match the name attribute of 
one of the item definitions within the enumeration (see below). 

* An optional attribute `serialize_type` giving the numeric type
of the enumeration when serializing.

If the attribute `namespace` is missing, then the type is
placed in the global namespace.

If the attribute `default` is missing, then the value of the
enumeration is set to 0.

If the attribute `serialize_type` is missing, then the serialization type is
set to FwEnumStoreType.

_Examples:_ Here is an XML enumeration `E` in the global namespace:

`<enum name="E">` ... `</enum>`

Here is an XML enumeration `E` in the namespace `A::B`:

`<enum name="E" namespace="A::B">` ... `</enum>`

Here is an XML enumeration `E` in the global namespace with default value Item2 
(Item2 is assumed to be the name attribute of one of the item definitions in the enum):

`<enum name="E" default="Item2">` ... `</enum>`


Here is an XML enumeration `E` in the global namespace with serialization type
U64:

`<enum name="E" serialize_type="U64">` ... `</enum>`

**Enum children:**
*enum_children* consists of the following, in any order:

* An optional node `comment` containing comment text.

  `<comment>` *comment_text* `</comment>`

  The comment text becomes a comment in the generated C++ code.
  It is attached to the C++ class that defines the enumeration.

* One or more instances of *item_definition*, described below.

**Item definition:**
*item_definition* defines an enumerated constant.
It is an XML node named `item` with the following attributes:

* An attribute `name` giving the name of the enumerated
constant.

* An optional attribute `value` assigning an integer value
to the enumerated constant.
If the `value` attribute is missing, then the value is assigned
in the ordinary way for C and C++ enumerations (i.e., zero for the first
constant, otherwise one more than the value assigned to
the previous constant).

* An optional attribute `comment` giving comment text.
The text becomes a comment in the generated C++ code.
It is attached to the enumerated constant definition.

_Examples:_ Here is an enumerated constant with a name only:

`<item name="ON"/>`

Here is an enumerated constant with a name, value, and comment:

`<item name="OFF" value="0" comment="The off state"/>`

### XML-Specified Array Type

#### Motivation

As discussed above, a member of a Serializable type can be an array of elements
of some other type.
For example, you can create a file `ACSTelemetrySerializable.xml` containing
this specification:

```xml
<serializable name="ACSTelemetry">
  <members>
    <member name="attitudeError" type="F32">
    <member name="wheelSpeeds" type="U32" size="3">
    </member>
  </members>
</serializable>
```

This file defines a Serializable type `ACSTelemetry`
with the following members:

* A member `attitudeError` of type `F32`.

* A member `wheelSpeeds` whose type is an array of three values, each of type
  `U32`.

Alternatively, you can specify a named array type *A* in a separate
XML file.
Then you can do the following:

1. Generate a C++ representation of *A* that you can include in C++
files and use on its own.

2. Use the XML representation of *A* in Serializable XML types, in other Array
XML types, in port arguments, in telemetry channels, and in event arguments.

As an example, you can create a file `WheelSpeedsArrayAi.xml` that specifies an
array of three `U32` values, like this:

```xml
<array name="WheelSpeeds">
  <type>U32</type>
  <size>3</size>
  <format>%u</format>
  <default>
    <value>0</value>
    <value>0</value>
    <value>0</value>
  </default>
</array>
```

By running the code generator on this file, you can generate C++
files `WheelSpeedsAc.hpp` and `WheelSpeedsAc.cpp`
that define a C++ class `WheelSpeeds` representing this type.
Anywhere that you include `WheelSpeedsEnumAc.hpp` in your C++ code, you can
use the class `WheelSpeeds`.
If you import `WheelSpeedsArrayAi.xml` into another XML definition,
then you can use the type `WheelSpeeds` there.

To use an XML array type *A* in another XML definition *D*,
enclose the name of the file that defines *A* in an XML tag
`import_array_type`.
As an example, you can revise the definition of the Serializable type
`ACSTelemetry` as follows:

```xml
<serializable name="ACSTelemetry">
  <import_array_type>WheelSpeedsArrayAi.xml</import_array_type>
  <members>
    <member name="attitudeError" type="F32"/>
    <member name="wheelSpeeds" type="WheelSpeeds"/>
  </members>
</serializable>
```

This specification defines an XML Serializable type with two members:

1. Member `attitudeError` of type `F32`.

1. Member `wheelSpeeds` of type `WheelSpeeds`.

Notice that whereas before `wheelSpeeds` was directly declared as
an array of three `U32` values, here it is given type `WheelSpeeds`,
which is defined in a separate XML specification as an array
of three `U32` values.

As another example, if you import file `WheelSpeedsArrayAi.xml` into the
definition of a component *C*, then you can use the type `WheelSpeeds` in the
telemetry dictionary for *C*.
When a value of type `WheelSpeeds` is emitted as telemetry, the GDS
will display it as an array of three values.

#### Specification

**File name:** An XML array type *A* must
be defined in a file with the name *A* `ArrayAi.xml`.
For example, the XML array type `WheelSpeeds`
must be defined in a file named `WheelSpeedsArrayAi.xml`.

**Top-level structure:** An XML array type is an XML node named `array`
with attributes *array_attributes* and children *array_children*.

`<array` *array_attributes* `>` *array_children* `</array>`

**Array attributes:** *array_attributes* consists of the following:

* An attribute `name` giving the name of the array type.

* An optional attribute `namespace` giving the enclosing namespace
of the array type.
The namespace consists of one or more identifiers separated by `::`.

If the attribute `namespace` is missing, then the type is
placed in the global namespace.

_Examples:_ Here is an XML array `A` in the global namespace:

`<array name="A">` ... `</array>`

Here is an XML array `A` in the namespace `B::C`:

`<array name="A" namespace="B::C">` ... `</array>`

**Array children:**
*array_children* consists of the following, in any order:

* An optional node `comment` containing comment text.

  `<comment>` *comment_text* `</comment>`

  The comment text becomes a comment in the generated C++ code.
  It is attached to the C++ class that defines the array.

* Zero or more of any of the following nodes:

   * `<include_header>` *header_file* `</include_header>` for including C++ header files
     into the generated C++.

   * `<import_serializable_type>` *serializable_xml_file* `</import_serializable_type>`
      for importing XML-specified serializable types.

   * `<import_enum_type>` *enum_xml_file* `</import_enum_type>` for importing
     XML-specified enum types.

   * `<import_array_type>` *array_xml_file* `</import_array_type>` for
     importing XML-specified array types.

* A node `format` providing a single format string to be applied to each array element.

  `<format>` *format_string* `</format>`

  *format_string* must contain a single conversion specifier starting with `%`.
  The conversion specifier must be legal both for C and C++ `printf` and for
  Python, considering the array element type.
  For example, if the array element type is `U32`, then `<format>%u</format>`
  is a valid
  format specifier. So is `<format>%u seconds</time>`.
  `<format>%s</format>` is not a legal format specifier in this case, because
  the string format `%s`
  is not valid for type `U32`.

* A node `type` consisting of attributes *type_attributes* and text *type*.

  `<type` *size_attribute_opt* `>` *type* `</type>`

  *size_attribute_opt* is an optional attribute `size` specifying a decimal
  integer size.
  The `size` attribute is valid only if the element type is `string`,
  and it is required in this case.
  It specifies the size of the string representation.

  *type* is text specifying the type of each array element.
  It must be an F Prime built-in type such as `U32` or a named type.
  A named type is (1) the name of an XML-specified type (Serializable, Enum, or Array)
  or (2) the name of a C++ class included with `include_header`.

  *Examples:*

    * `<type>U32</type>` specifies the built-in type `U32`

    * `<type>T</type>` specifies the named type `T`.
      `T` must be (1) included via `include_header` or (2) imported
      via `import_serializable_type`, `import_enum_type`, or
      `import_array_type`.

    * `<type size="40">string</type>` specifies a string
      type of size 40.

* A node `size` specifying the size of the array as a decimal integer.

  `<size>` *integer* `</size>`

* A node `default` with one or more child nodes `value`.

  `<default>` _value_ ... `</default>`

  The format of _value_ is described below.
  There must be one value for each element of the array.

**Value:** The node `value` specifies a default value for an array
element.
The value must be text that, when inserted into the generated C++ code,
represents a default value of the correct type.

*Examples:*

* If the array element type is `U32`, then `0` is a correct default value.

* Suppose the array element type is an array `A` of three U32 values.
In the generated C++ code, `A` is a class with a three-argument constructor,
and each argument initializes an array element.
Therefore `A(1, 2, 3)` is a correct default value.


### Hand-coded Serializable

A hand-coded serializable is useful in the case where the type is not
easily expressed as a collection of basic types or if the user is
employing an external library with types already defined. A user may
also wish to attach special processing functions to a class. The pattern
for writing C++ serializable classes is to declare a class that is
derived from the framework base class Fw::Serializable. That type can be
found in Fw/Types/Serializable.hpp. An example can be found in
Autocoders/templates/ExampleType.hpp. The method is as follows:

1. Define the class. It should be derived from Fw::Serializable.

1. Declare the two base class pure virtual functions: serialize() and
   deserialize(). These functions provide a buffer as a destination for
   the serialized form of the data in the type.

1. Implement the functions. The buffer base class that is the argument
   to those functions provides a number of helper functions for
   serializing and deserializing the basic types in the table above.
   Those functions are specified in the SerializeBufferBase class type
   in the same header file. For each member that the developer wishes
   to save, call the serialization functions. The members can be
   serialized in any order. In the worst case, a raw buffer version is
   provided for just doing a bulk copy of the data if serializing
   individual members is not feasible. The deserialization function
   should deserialize the data in the order it was serialized.

1. Add an enumeration with a single member named SERIALIZED\_SIZE. The
   value of that member should be the sum of the sizes of all the
   members. It can be done with the sizeof() function that is available
   to all C/C++ compilers. Optionally, a type identifier can be added
   so that a sanity check can be done when the data are deserialized.
   That type should be serialized, and then checked against the
   enumeration when it is deserialized.

1. Implement copy constructors and equal operators. If the type is
   passed by value, the developer should write these functions if the
   default is not sufficient.

1. Implement any custom features.

Once the class is completed, it can be included in port definitions.

#### Constraints

##### Ground Interfaces

Hand-coded user types cannot be used in XML definitions for types that
will be sent to and from the ground system, since the ground system
requires all type definitions to be in XML. If a developer wants to use
a type that has special functions in a ground interface, it can be
defined with the members only in XML and generated by the code
generator. The developer can then define a derived class with the
behavior.

## Port

Ports are the interfaces between components. Components invoke
functionality in other components by invoking methods on their output
ports instead of invoking the components directly. Input ports invoke
methods defined by the components and registered at run time. Ports are
fully specified in XML files. There is no developer-written code, but
the XML specifications can include argument types defined by the user as
long as they are serializable as described in Section 6.6.1.2. The
process of defining a port is to specify the arguments to the method in
the XML file. In order for the build system to detect that a file
contains the XML for a port type, the file must follow the naming
convention \<SomeName\>PortAi.xml. An example of this can be found in
Autocoders/templates/ExamplePortAi.xml. Table 17 describes the XML tags
and attributes.

**Table 17.** Port XML
specification.

| Tag                        | Attribute | Description                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            |
| -------------------------- | --------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| interface                  |           | The outermost tag that indicates a port is being defined.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              |
| interface                  | namespace | The C++ namespace for the port class (optional).                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       |
| interface                  | name      | The class name for the port.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           |
| comment                    |           | Used for a comment describing the port. Is placed as a Doxygen-compatible tag in the class declaration.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                |
| args                       |           | Optional. Starts the region of the declaration where port arguments are specified.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     |
| arg                        |           | Defines an argument to the port method.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                |
| arg                        | type      | The type of the argument. Should be a built-in type, ENUM, string, an XML-specified type, or a user-written serializable type. A string type should be used if a text string is the argument. If the type is serial, the port is an untyped serial port that can be connected to any typed port for the purpose of serializing the call. See the Hub pattern in the architectural description document for usage.                                                                                                                                                                                                                                                                             |
| arg                        | name      | Defines the argument name.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             |
| arg                        | size      | Specifies the size of the argument if it is of type string.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            |
| arg                        | pass\_by  | Optional. Specifies how the argument should be passed to a handler function. Values can be `pointer` or `reference`. For a synchronous port handler, the default behavior (with no `pass_by` attribute) is to pass by value for primitive values, otherwise by const reference. `pass_by="pointer"` causes a pointer to the data to be passed. `pass_by="reference"` causes a mutable reference to be passed. For an asynchronous port handler, `pass_by="pointer"` causes a pointer to the data to be passed. Otherwise, the data itself is serialized, copied into and out of the queue, and deserialized. The use of pointers and references carries the usual responsibility for understanding the scope and lifetime of the memory behind the pointer, as the pointer or reference may be held by another component past the duration of the port call. |
| enum                       |           | Specifies an enumeration when the argument type=ENUM.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  |
| enum                       | name      | Enumeration type name.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 |
| item                       |           | Specifies a member of the enumeration.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 |
| item                       | name      | Specifies the name of the enumeration member.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          |
| item                       | value     | Assigns a value to the enumeration member. Member values in the enumeration follow C enumeration rules if not specified.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               |
| item                       | comment   | A comment about the member. Becomes a Doxygen tag.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     |
| return                     |           | Optional. Specifies that the method will return a value.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               |
| return                     | type      | Specifies the type of the return. Can be a built-in type.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 |
| return                     | pass\_by  | Specifies whether the argument should be passed by value or by reference or pointer.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   |
| import\_serializable\_type |           | Imports an XML definition of another serializable type for use in the definition.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      |
| import\_enum\_type         |           | Imports an XML definition of an enumeration type.|
| import\_array\_type        |           | Imports an XML definition of an array type.|
| include\_header            |           | Includes a C/C++ user-written header for argument types.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               |

### Constraints

The following constraints are on port XML definitions.

#### Serialization

Port calls are serialized for various reasons by components, such as for
copying data to put on a message queue. A port call cannot be serialized
if it contains return values, since the serialized arguments are passed
only to the connected port or component and no return data are provided.

## Components

Components are the places in the architecture where all the behavior
resides. Incoming port calls deliver data and actions to be acted on,
and output ports are used to invoke functions outside the component. In
addition, the components define the interfaces used to communicate with
ground software. Components can be used for a variety of purposes such
as hardware drivers, state machines, device managers, and data
management. The process of defining a component is to specify the ports
and ground data interfaces in XML. The developer then implements a
derived class that inherits from the base class and implements the
interfaces as methods in the class (see Section 6.7). In order for the
build system to detect that a file contains the XML for a component
type, the file must follow the naming convention
\<SomeName\>ComponentAi.xml. An example of this can be found in
Autocoders/templates/ExampleComponentAi.xml. Table 18 describes the XML
tags and attributes.

**Table 18.** Component XML
specification.

| Tag                        | Attribute       | Description                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     |                                                                                                                                                                        |
| -------------------------- | --------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| component                  |                 | The outermost tag that indicates a component is being defined.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  |                                                                                                                                                                        |
| component                  | namespace       | The C++ namespace for the component class (optional).                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           |                                                                                                                                                                        |
| component                  | name            | The class name for the component.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               |                                                                                                                                                                        |
| component                  | kind            | The type of component. Can be passive, queued, or active. A passive component has no thread or queue. A queued component has a message queue, but no thread. A component on another thread must invoke a synchronous input interface (see below) to get messages from the queue. An active component has a thread and unloads its message queue as the thread is scheduled and as port invocation messages arrive.                                                                                                                                                             |                                                                                                                                                                        |
| component                  | modeler         | When the attribute is “true,” the autocoder does not automatically create ports for commands, telemetry, events, and parameters. If it is “true,” those ports must be declared in the port section with the “role” attribute. See the description below.                                                                                                                                                                                                                                                                                                                            |                                                                                                                                                                        |
| import\_dictionary         |                 | Imports a ground dictionary defined outside the component XML that conforms to the command, telemetry, event, and parameter entries below. This allows external tools written by projects to generate dictionaries.                                                                                                                                                                                                                                                                                                                                                             |                                                                                                                                                                        |
| import\_port\_type         |                 | Imports an XML definition of a port type used by the component.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 |                                                                                                                                                                        |
| import\_serializable\_type |                 | Imports an XML definition of a serializable type for use in the component interfaces.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           |                                                                                                                                                                        |
| import\_enum\_type         |           | Imports an XML definition of an enumeration type.|
| import\_array\_type        |           | Imports an XML definition of an array type.|
| comment                    |                 | Used for a comment describing the component. Is placed as a Doxygen-compatible tag in the class declaration.                                                                                                                                                                                                                                                                                                                                                                                                                                                                    |                                                                                                                                                                        |
| ports                      |                 | Defines the section of the component definition where ports are defined.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        |                                                                                                                                                                        |
| port                       |                 | Starts the description of a port.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               |                                                                                                                                                                        |
| port                       | name            | Defines the name of the port.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   |                                                                                                                                                                        |
| port                       | data\_type      | Defines the type of the port. The XML file containing the type of the port must be provided via the import\_port\_type tag.                                                                                                                                                                                                                                                                                                                                                                                                                                                     |                                                                                                                                                                        |
| port                       | kind            | Defines the synchronicity and direction of the port. The port can be of the following kinds:                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    |                                                                                                                                                                        |
|                            |                 | **Kind**                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        | **Attributes**                                                                                                                                                         |
|                            |                 | sync\_input                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     | Invokes the derived class methods directly.                                                                                                                            |
|                            |                 | guarded\_input                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  | Invokes the derived class methods after locking a mutex shared by all guarded ports and commands.                                                                      |
|                            |                 | async\_input                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    | Creates a message with the serialized arguments of the port call. When the message is dequeued, the arguments are deserialized and the derived class method is called. |
|                            |                 | output                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          | Port is an output port that is invoked from the logic in the derived class.                                                                                            |
| port                       | priority        | The priority of the invocation. Only used for asynchronous ports, and specifies the priority of the message in the underlying message queue if priorities are supported by the target OS. Range is OS-dependent.                                                                                                                                                                                                                                                                                                                                                                |                                                                                                                                                                        |
| port                       | max\_number     | Specifies the number of ports of this type. This allows multiple callers to the same type of port if knowing the source is necessary. Can be specified as an Fw/Cfg/AcConstants.ini file $VARIABLE value.                                                                                                                                                                                                                                                                                                                                                                      |                                                                                                                                                                        |
| port                       | full            | Specifies the behavior for async ports when the message queue is full. One of *drop*, *block*, or *assert*, where *assert* is the default.                                                                                                                                                                                                                                                                                                                                                                                                                                      |                                                                                                                                                                        |
| port                       | role            | Specifies the role of the port when the modeler=true.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           |                                                                                                                                                                        |
| commands                   |                 | Optional. Starts the section where software commands are defined.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               |                                                                                                                                                                        |
| commands                   | opcode\_base    | Defines the base value for the opcodes in the commands. If this is specified, all opcodes will be added to this value. If it is missing, opcodes will be absolute. This tag can also have a variable of the form $VARIABLE referring to values in Fw/Cfg/AcConstants.ini.                                                                                                                                                                                                                                                                                                      |                                                                                                                                                                        |
| command                    |                 | Starts the definition for a particular command.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 |                                                                                                                                                                        |
| command                    | mnemonic        | Defines a text label for the command. Can be alphanumeric with “\_” separators, but no spaces.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  |                                                                                                                                                                        |
| command                    | opcode          | Defines an integer that represents the opcode used to decode the command. Should be a C-compilable constant.                                                                                                                                                                                                                                                                                                                                                                                                                                                                    |                                                                                                                                                                        |
| command                    | kind            | Defines the synchronicity of the command. The command can be of the following kinds:                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            |                                                                                                                                                                        |
|                            |                 | **Kind**                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        | **Attributes**                                                                                                                                                         |
|                            |                 | sync                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            | Invokes the derived class methods directly.                                                                                                                            |
|                            |                 | guarded                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         | Invokes the derived class methods after locking a mutex shared by all guarded ports and commands.                                                                      |
|                            |                 | async                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           | Creates a message with the serialized arguments of the port call. When the message is dequeued, the arguments are deserialized and the derived class method is called. |
| command                    | priority        | Sets command message priority if message is asynchronous, ignored otherwise.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    |                                                                                                                                                                        |
| command                    | full            | Specifies the behavior for async commands when the message queue is full. One of *drop*, *block*, or *assert*, where *assert* is the default.                                                                                                                                                                                                                                                                                                                                                                                                                                   |                                                                                                                                                                        |
| args                       |                 | Optional. Starts the region of the declaration where command arguments are specified.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           |                                                                                                                                                                        |
| arg                        |                 | Defines an argument in the command.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             |                                                                                                                                                                        |
| arg                        | type            | The type of the argument. Should be a built-in type. A string type should be used if a text string is the argument.                                                                                                                                                                                                                                                                                                                                                                                         |                                                                                                                                                                        |
| arg                        | name            | Defines the argument name.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      |                                                                                                                                                                        |
| arg                        | size            | Specifies the size of the argument if it is of type string.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     |                                                                                                                                                                        |
| enum                       |                 | Specifies an enumeration when the argument type=ENUM.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           |                                                                                                                                                                        |
| enum                       | name            | Enumeration type name.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          |                                                                                                                                                                        |
| item                       |                 | Specifies a member of the enumeration.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          |                                                                                                                                                                        |
| item                       | name            | Specifies the name of the enumeration member.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   |                                                                                                                                                                        |
| item                       | value           | Assigns a value to the enumeration member. Member values in the enumeration follow C enumeration rules if not specified.                                                                                                                                                                                                                                                                                                                                                                                                                                                        |                                                                                                                                                                        |
| item                       | comment         | A comment about the member. Becomes a Doxygen tag.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              |                                                                                                                                                                        |
| telemetry                  |                 | Optional. Specifies the section that defines the channelized telemetry.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         |                                                                                                                                                                        |
| telemetry                  | telemetry\_base | Defines the base value for the channel IDs. If this is specified, all channel IDs will be added to this value. If it is missing, channel IDs will be absolute. This tag can also have a variable of the form $VARIABLE referring to values in Fw/Cfg/AcConstants.ini.                                                                                                                                                                                                                                                                                                          |                                                                                                                                                                        |
| channel                    |                 | Starts the definition for a telemetry channel.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  |                                                                                                                                                                        |
| channel                    | id              | Specifies a numerical value identifying the channel.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            |                                                                                                                                                                        |
| channel                    | name            | A text string with the channel name. Cannot contain spaces.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     |                                                                                                                                                                        |
| channel                    | data\_type      | Specifies the type of the channel. Should be a built-in type, ENUM, string, or an XML-specified serializable. A string type should be used if a text string is the argument.                                                                                                                                                                                                                                                                                                                                                                                |                                                                                                                                                                        |
| channel                    | size            | If the channel type is string, specifies the size of the string.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                |                                                                                                                                                                        |
| channel                    | abbrev          | An abbreviation for the channel. Needed for the AMMOS Mission Data Processing and Control System (AMPCS), a ground data system to display telemetry and events from spacecraft.                                                                                                                                                                                                                                                                                                                                                                                                 |                                                                                                                                                                        |
| channel                    | update          | If the channel should be always updated, or only on change. Values are ALWAYS or ON\_CHANGE.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    |                                                                                                                                                                        |
| channel                    | format\_string  | A format string specifier for displaying the channel value.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     |                                                                                                                                                                        |
| comment                    |                 | A comment describing the channel.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               |                                                                                                                                                                        |
| enum                       |                 | Specifies an enumeration when the channel type=ENUM.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            |                                                                                                                                                                        |
| enum                       | name            | Enumeration type name.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          |                                                                                                                                                                        |
| item                       |                 | Specifies a member of the enumeration.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          |                                                                                                                                                                        |
| item                       | name            | Specifies the name of the enumeration member.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   |                                                                                                                                                                        |
| item                       | value           | Assigns a value to the enumeration member. Member values in the enumeration follow C enumeration rules if not specified.                                                                                                                                                                                                                                                                                                                                                                                                                                                        |                                                                                                                                                                        |
| item                       | comment         | A comment about the member. Becomes a Doxygen tag.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              |                                                                                                                                                                        |
| parameters                 |                 | Optional. Specifies the section that defines parameters for the component.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      |                                                                                                                                                                        |
| parameters                 | parameter\_base | Defines the base value for the parameter IDs. If this is specified, all parameter IDs will be added to this value. If it is missing, parameter IDs will be absolute. This tag can also have a variable of the form $VARIABLE referring to values in Fw/Cfg/AcConstants.ini.                                                                                                                                                                                                                                                                                                    |                                                                                                                                                                        |
| parameters                 | opcode\_base    | Defines the base value for the opcodes in the parameter set and saves commands. If this is specified, all opcodes will be added to this value. If it is missing, opcodes will be absolute. This tag can also have a variable of the form $VARIABLE referring to values in Fw/Cfg/AcConstants.ini.                                                                                                                                                                                                                                                                               |                                                                                                                                                                        |
| parameter                  |                 | Starts the definition for a parameter.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          |                                                                                                                                                                        |
| parameter                  | id              | Specifies a numeric value that represents the parameter.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        |                                                                                                                                                                        |
| parameter                  | name            | Specifies the name of the parameter.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            |                                                                                                                                                                        |
| parameter                  | data\_type      | Specifies the type of the parameter. Should be a built-in type, ENUM, string, or an XML-specified serializable. A string type should be used if a text string is the argument.                                                                                                                                                                                                                                                                                                                                                                              |                                                                                                                                                                        |
| parameter                  | size            | Specifies the size of the parameter if it is of type string.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    |                                                                                                                                                                        |
| parameter                  | default         | Specifies a default value for the parameter if the parameter is unable to be retrieved from non-volatile storage. Only for built-in types.                                                                                                                                                                                                                                                                                                                                                                                                                                      |                                                                                                                                                                        |
| parameter                  | comment         | A comment describing the parameter.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             |                                                                                                                                                                        |
| parameter                  | set\_opcode     | Command opcode used to set parameter.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           |                                                                                                                                                                        |
| parameter                  | save\_opcode    | Command opcode used to save parameter.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          |                                                                                                                                                                        |
| enum                       |                 | Specifies an enumeration when the parameter type=ENUM.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          |                                                                                                                                                                        |
| enum                       | name            | Enumeration type name.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          |                                                                                                                                                                        |
| item                       |                 | Specifies a member of the enumeration.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          |                                                                                                                                                                        |
| item                       | name            | Specifies the name of the enumeration member.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   |                                                                                                                                                                        |
| item                       | value           | Assigns a value to the enumeration member. Member values in the enumeration follow C enumeration rules if not specified.                                                                                                                                                                                                                                                                                                                                                                                                                                                        |                                                                                                                                                                        |
| item                       | comment         | A comment about the member. Becomes a Doxygen tag.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              |                                                                                                                                                                        |
| events                     |                 | Optional. Specifies the section that defines events for the component.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          |                                                                                                                                                                        |
| events                     | event\_base     | Defines the base value for the event IDs. If this is specified, all event IDs will be added to this value. If it is missing, event IDs will be absolute. This tag can also have a variable of the form $VARIABLE referring to values in Fw/Cfg/AcConstants.ini.                                                                                                                                                                                                                                                                                                                |                                                                                                                                                                        |
| event                      |                 | Starts the definition for an event.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             |                                                                                                                                                                        |
| event                      | id              | Specifies a numeric value that represents the event.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            |                                                                                                                                                                        |
| event                      | name            | Specifies the name of the event.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                |                                                                                                                                                                        |
| event                      | severity        | Specifies the severity of the event. The values can be:                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         |                                                                                                                                                                        |
|                            |                 | **Value**                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       | **Meaning**                                                                                                                                                            |
|                            |                 | DIAGNOSTIC                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      | Software debugging information. Meant for development.                                                                                                                 |
|                            |                 | ACTIVITY\_LO                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    | Low-priority events related to software execution.                                                                                                                     |
|                            |                 | ACTIVITY\_HI                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    | Higher priority events related to software execution.                                                                                                                  |
|                            |                 | COMMAND                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         | Events related to command execution. Should be reserved for the command dispatcher and sequencer.                                                                          |
|                            |                 | WARNING\_LO                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     | Error conditions that are of low importance.                                                                                                                           |
|                            |                 | WARNING\_LO                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     | Error conditions that are of critical importance.                                                                                                                      |
|                            |                 | FATAL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           | An error condition was encountered that the software cannot recover from.                                                                                              |
| event                      | format\_string  | A C-style format string to print a message corresponding to the event. Used for displaying the event in the command/data handling software as well as the optional text logging in the software. (See Section 9.12.)                                                                                                                                                                                                                                                                                                                                                            |                                                                                                                                                                        |
| event                      | throttle        | Maximum number of events that will be issued before more are prevented. Once the limit has been reached, the throttle must be cleared before more can be issued. Non-negative integer.                                                                                                                                                                                                                                                                                                                                                                                          |                                                                                                                                                                        |
| comment                    |                 | A comment describing the event.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 |                                                                                                                                                                        |
| args                       |                 | Starts the region of the declaration where event arguments are specified.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       |                                                                                                                                                                        |
| arg                        |                 | Defines an argument in the event.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               |                                                                                                                                                                        |
| arg                        | type            | The type of the argument. Should be a built-in type, ENUM, string, or an XML-specified serializable. A string type should be used if a text string is the argument.                                                                                                                                                                                                                                                                                                                                                                                         |                                                                                                                                                                        |
| arg                        | name            | Defines the argument name.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      |                                                                                                                                                                        |
| arg                        | size            | Specifies the size of the argument if it is of type string.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     |                                                                                                                                                                        |
| internal\_interfaces       |                 | Optional. Specifies an internal interface for the component. Internal interfaces are functions that can be called internally from implementation code. These functions will dispatch a message in the same fashion that asynchronous ports and commands do. The developer implements a handler in the same way, and that handler is called on the thread of an active or queued component. Internal interfaces cannot be specified for a passive component since there is no message queue. A typical use for an internal interface would be for an interrupt service routine. |                                                                                                                                                                        |
| internal\_interface        |                 | Specifies an internal interface call.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           |                                                                                                                                                                        |
| internal\_interface        | priority        | Sets internal interface message priority if the message is asynchronous, ignored otherwise.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         |                                                                                                                                                                        |
| internal\_interface        | full            | Specifies the behavior for internal interfaces when the message queue is full. One of *drop*, *block*, or *assert*, where *assert* is the default.                                                                                                                                                                                                                                                                                                                                                                                                                              |                                                                                                                                                                        |
| comment                    |                 | A comment describing the interface.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             |                                                                                                                                                                        |
| args                       |                 | Starts the region of the declaration where interface arguments are specified.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   |                                                                                                                                                                        |
| arg                        |                 | Defines an argument in the interface.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           |                                                                                                                                                                        |
| arg                        | type            | The type of the argument. Should be a built-in type, ENUM, string, or an XML-specified serializable. A string type should be used if a text string is the argument.                                                                                                                                                                                                                                                                                                                                                                                         |                                                                                                                                                                        |
| arg                        | name            | Defines the argument name.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      |                                                                                                                                                                        |
| arg                        | size            | Specifies the size of the argument if it is of type string.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     |                                                                                                                                                                        |

**NOTE:** This table describes the hierarchical XML structure. Repeated
rows are usable under multiple different parents, in the same format.
For example, commands can define inner enumeration elements, events can
define enumerations, and channels can define enumerations. Thus, some
rows are repeated to show that this is possible.

### Constraints

The following constraints are on components.

#### Passive Components

Passive components cannot have asynchronous input ports or commands,
since there is no queue for messaging.

#### Queued Components

Queued components must have at least one synchronous or guarded input
port or synchronous command so that a calling thread can use the port to
retrieve port call messages from the message queue.

Queued components must have at least one asynchronous input port,
command, or internal interface, or there would be no purpose in making
the component queued.

#### Active Components

Active components must have at least one asynchronous input port,
command, or internal interface, or there would be no purpose in making
the component active or have a message queue.

#### Command and Telemetry Interfaces

The component XML is parsed by the command and telemetry system in order
to construct the data storage and display application. For this reason,
only the built-in types or types represented in XML can be
used.

## Command/Telemetry Ports

When declaring commands, telemetry, events, and parameters for a
component, the code generator automatically creates the correct set of
ports for those interfaces. Those ports are based on normal XML
definitions of ports, so those port types can be used as normal ports in
other components. As an example, if a component needs to implement
commands, the XML would declare those commands as shown in Table 1.
Another component needs to act as a command dispatcher to send commands
to that component, so the dispatcher component would add an output port
of the command type. The dispatcher is handling commands as a generic
port invocation rather than deserializing the command arguments as the
generated code does in the component that declares the commands. This
allows writing components to process commands and telemetry generically
without having to treat command and telemetry ports as special cases.
The port definition XML files can be included in component XML
definitions in the same manner as other ports. Table 19 provides a list
of the types, file names, and descriptions of the ports used for the
command and telemetry ports.

**Table 19.** Port
types.

| Port Type            | XML File                     | Description                                                                                                                               |
| -------------------- | ---------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------- |
| **Commands**         |                              |                                                                                                                                           |
| Command              | Fw/Cmd/CmdPortAi.xml         | A port that passes a serialized command to a component.                                                                                   |
| Command Response     | Fw/Cmd/CmdResponsePortAi.xml | A port that passes the completion status of a command.                                                                                    |
| Command Registration | Fw/Cmd/CmdRegPortAi.xml      | A port used to request registration of a command. Used during initialization to tell a command dispatcher where to send specific opcodes. |
| **Telemetry**        |                              |                                                                                                                                           |
| Telemetry            | Fw/Tlm/TlmPortAi.xml         | A port that passes a serialized telemetry value.                                                                                          |
| Time                 | Fw/Time/TimePortAi.xml       | A port that returns a time value for time stamping the telemetry.                                                                         |
| **Events**           |                              |                                                                                                                                           |
| Log                  | Fw/Log/LogPortAi.xml         | A port that passes a serialized event.                                                                                                    |
| LogText              | Fw/Log/LogTextPortAi.xml     | A port that passes the text form of an event. Can be disabled via configuration of the architecture.                                      |
| Time                 | Fw/Time/TimePortAi.xml       | A port that returns a time value for time stamping the telemetry.                                                                         |
| **Parameters**       |                              |                                                                                                                                           |
| Parameter            | Fw/Prm/PrmPortAi.xml         | A port that returns a serialize parameter value                                                                                           |

## Topologies

The topology (or interconnection) of components can be implemented by
interconnecting them manually (see Section 6.8) or by specifying them
via a topology XML file. An example of this can be seen in
Ref/Top/RefTopologyAppAi.xml. The file will be processed by the
autocoder if it has the ending AppAi.xml. Table 20 provides the XML
specification for topologies.

**Table 20.** Topology XML
specification.

| Tag                     | Attribute        | Description                                                                                                                                                                  |
| ----------------------- | ---------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| deployment              |                  | The outermost tag that indicates a topology is being defined.                                                                                                                |
| assembly                |                  | Alternate declaration for deployment.                                                                                                                                        |
| deployment              | name             | The name of the deployment.                                                                                                                                                  |
| import\_component\_type |                  | Imports the XML file that defines a component used in the topology.                                                                                                          |
| instance                |                  | Defines a component instance.                                                                                                                                                |
| instance                | name             | Name of the component instance. This instance name must match a component object declared in the C++ code.                                                                   |
| instance                | namespace        | C++ namespace of component implementation type.                                                                                                                              |
| instance                | type             | C++ type of implementation class                                                                                                                                             |
| instance                | base\_id         | The starting ID value for commands, events, and telemetry for this instance of the component. Used to construct dictionary for ground system.                                |
| instance                | base\_id\_window | A bookkeeping attribute that the modeler uses to space out the base\_id values. It can be omitted if the base\_ids are spaced enough to cover the id range in the component. |
| connection              |                  | Defines a connection between two component ports.                                                                                                                            |
| connection              | name             | Name of connection.                                                                                                                                                          |
| source                  |                  | Defines the source of the connection.                                                                                                                                        |
| source                  | component        | Defines source component. Must match an instance in instance section above.                                                                                                  |
| source                  | port             | Defines the source port on the component.                                                                                                                                            |
| source                  | type             | Source port type.                                                                                                                                                            |
| source                  | num              | Source port number if multiple port instances.                                                                                                                               |
| target                  | component        | Defines target component. Must match an instance in instance section above.                                                                                                  |
| target                  | port             | Defines the target port on the component.                                                                                                                                            |
| target                  | type             | Target port type.                                                                                                                                                            |
| target                  | num              | Target port number if multiple port instances.                                                                                                                               |

The autocoder will output a source file and header file following the
convention \<Deployment Name\>AppAc.cpp and .hpp. The header file
contains a function named construct\<architecture\>Architecture. This
function will call all the connection methods to connect the components.
The file requires a header Components.hpp in the directory where the XML
is defined that has declarations for the implementation class instances
in the connections.

### Constraints

The XML specification for the component requires a static declaration of
component instances that can be referred to by their object name. If
components are instantiated in other ways such as a heap, the manual
method can be used.