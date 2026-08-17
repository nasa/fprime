# Fw::Log / Fw::LogText Ports

## 1. Introduction

The `Fw::Log` port is used to pass a serialized form of an F´ event. It passes the ID, a time tag, the severity, and a buffer
containing the serialized arguments of the event. 

The `Fw::LogText` port is used to pass a printable text representation of an F´ event. It passes the ID, a time tag, 
the severity, and a string containing a description of the event.  

## 2. Design

### 2.1 Context

#### 2.1.1 Port Diagram

The `Fw::Log` and `Fw::LogText` ports have the following port diagram:

![`Fw::Log/LogText` Diagram](img/LogBDD.jpg "Fw::Log/LogText Port")

Both ports have the following arguments:

Argument | Description
----- | -----------
id | Event identifier 
timeTag | The system time the event happened
severity | The severity of the event (`Fw::LogSeverity`). 
 | FATAL - A condition has been encountered that makes the software unable to continue
 | WARNING_HI - A condition has been encountered that is a serious failure, but the software can continue
 | WARNING_LO - A condition has been encountered that is a failure, but the software is largely unaffected
 | COMMAND - An activity related to processing commands has happened
 | ACTIVITY_HI - An important nominal event has happened
 | ACTIVITY_LO - An unimportant nominal event has happened that is a subset of an important event
 | DIAGNOSTIC - A detailed list of events in an activity that normally do not need to be seen; used for debugging
args / text | Serialized event arguments (`Fw::Log`) or text version of the event (`Fw::LogText`)

#### 2.1.2 Serializables

##### 2.1.2.1 Fw::LogBuffer

The `Fw::LogBuffer` class represents a buffer to store a serialized parameter value.

##### 2.1.2.2 Fw::LogStringArg

The `Fw::LogStringArg` class is used by the logging autocoder when string arguments are declared.

## 3. Change Log

Date | Description
---- | -----------
9/3/2015 |  Initial Version
