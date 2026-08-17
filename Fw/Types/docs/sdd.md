# Fw::Type Classes and Types

## 1. Introduction

The `Fw::Types` module acts as a source of definition for types used in the architecture and in implementation code. 
It is meant to provide aliases to built-in types as well as a number of base classes and helper classes.

## 2. Type Descriptions

### 2.1 PolyType

The PolyType class is a polymorphic class that can hold a number of different built-in types. 
The user of the class can assign one of the types in `BasicTypes.hpp` to an instance of the class, and the value and an indication of the type will be stored.
If an attempt is made to retrieve a value type other than the one stored, an `Fw::Assert` will be called.
`Fw::PolyType` is a subtype of `Fw::Serializable,` so it can be passed via ports.

### 2.2 Other Types and Classes

In addition to `PolyType`, the module provides:

* Assertion machinery (`Assert.hpp`, `FW_ASSERT`) and assertion hook support
* String classes: `Fw::StringBase`, `Fw::String`, `Fw::ExternalString`, `Fw::StringTemplate`, and related utilities (`StringUtils.hpp`)
* Serialization support: `Fw::Serializable`, `Fw::SerialBufferBase`, `Fw::ExternalSerializeBuffer`, and `Fw::SerialBuffer`
* Byte array and buffer helpers (`ByteArray.hpp`)
* Memory allocators: `Fw::MemAllocator`, `Fw::MallocAllocator`
* `Fw::Optional`, a type that may or may not hold a value

## 3. Change Log

Date | Description
---- | -----------
6/24/2015 |  Initial Version



