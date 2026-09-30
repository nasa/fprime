# Fw::Port Classes

## 1. Introduction

The `Fw::Port` module contains the base classes for input and output ports in the architecture. 

## 2. Type Descriptions

### 2.1 Fw::PortBase

The `Fw::PortBase` class is the port base class in the ISF class hierarchy. It is the base class for all ports.  

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



