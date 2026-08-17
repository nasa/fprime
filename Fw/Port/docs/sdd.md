# Fw::Port Classes

## 1. Introduction

The `Fw::Port` module contains the base classes for input and output ports in the architecture. 

## 2. Type Descriptions

### 2.1 Fw::PortBase

The `Fw::PortBase` class is the port base class in the F´ class hierarchy. It is the base class for all ports.  

### 2.2 Fw::InputPortBase

The `Fw::InputPortBase` class is the base class for all input ports. It stores a pointer to the component that owns
the port and the port number, which are used when invoking the component's handler for the port.

### 2.3 Fw::OutputPortBase

The `Fw::OutputPortBase` class is the base class for all output ports. It supports connecting the output port to an
input port either directly (typed connection) or via a serialized connection when serialization is enabled.

### 2.4 Fw::InputSerializePort

The `Fw::InputSerializePort` class is an input port that receives serialized buffers. It is used to connect
serialized output ports to components that handle serialized data (available when `FW_PORT_SERIALIZATION` is enabled).

### 2.5 Fw::OutputSerializePort

The `Fw::OutputSerializePort` class is an output port that sends serialized buffers. It allows a typed input port to
be driven from a serialized connection (available when `FW_PORT_SERIALIZATION` is enabled).

## 3. Change Log

Date | Description
---- | -----------
4/24/2016 |  Initial Version
