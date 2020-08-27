# Fw::Obj Classes

## 1. Introduction

The `Fw::Obj` module contains the `Fw::ObjBase` base class that defines the root class for objects in the F´ architecture.
In addition, it provides a definition for a pure virtual base class `Fw::ObjRegistry` that is used to register objects
when they are initialized.

## 2. Type Descriptions

### 2.1 Fw::ObjBase

The `Fw::ObjBase` class is the base class in the F´ class hierarchy. It is the base class for all components 
and ports. It stores the object name as well as performs object registration if an object registry has been
registered with the class. 

### 2.2 Fw::ObjRegistry

The `Fw::ObjRegistry` class is a virtual base class for object registry. This is an optional feature that is
turned on or off with the `FW_OBJECT_REGISTRATION` macro, found in `FpConfig.hpp`. The concept is that
an object registry provides a way to track all the objects that have been instantiated in the system. The registry
can query any public functions in `Fw::Object` to get information on the instance. The actual method for storing
`Fw::Object` pointers and producing data on the instances is left to the derived classes.

## 3. Change Log

Date | Description
---- | -----------
4/24/2016 |  Initial Version



