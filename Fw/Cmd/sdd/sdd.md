# Fw::Cmd / Fw::CmdResponse / Fw::CmdReg Ports

## 1. Introduction

The `Fw::Cmd` port is used to send a command with encoded arguments to a component.

The `Fw::CmdResponse` port is used by components to report the completion status of a command.

The `Fw::CmdReg` port is used by components to register their set of command opcodes.

## 2. Design

### 2.1 Context

#### 2.1.1 Port Diagram

The `Fw::Cmd` ports and types have the following diagram:

![`Fw::Cmd` Diagram](img/CmdBDD.jpg "Fw::Cmd")

#### 2.1.2 Serializables

##### 2.1.2.1 Fw::PrmBuffer

The `Fw::CmdArgBuffer` class represents a buffer to store a serialized command argument list.

The `Fw::CmdString` class represents a string class used for string command arguments.

The `Fw::CmdPacket` class represents a packet containing a serialized command.


## 3. Change Log

Date | Description
---- | -----------
6/25/2015 |  Initial Version



