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

## 3. Change Log

Date | Description
---- | -----------
6/24/2015 |  Initial Version



