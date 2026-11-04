# Fw::Log / Fw::LogText Ports

## 1. Introduction

The `Fw::Log` port is used to pass a serialized form of an ISF Event. It passes the ID, a time tag, the severity, and a buffer
containing the serialized arguments of the event. 

The `Fw::LogText` port is used to pass a printable text representation of an ISF Event. It passes the ID, a time tag, 
the severity, and a string containing a description of the event.  

## 2. Design

### 2.1 Context

#### 2.1.1 Port Diagram

The `Fw::Log` and `Fw::LogText` ports have the following port diagram:

![`Fw::Log/LogText` Diagram](img/LogBDD.jpg "Fw::Log/LogText Port")

The `Fw::Log` port has the following arguments:

Argument | Description
----- | -----------
id | Event identifier 
timeTag | The system time the event happened
severity | The severity of the event. 
 | LOG_FATAL - A condition has been encountered that makes the software unable to continue
 | LOG_WARNING_HI - A condition has been encountered that is a serious failure, but the software can continue
 | LOG_WARNING_LO - A condition has been encountered that is a failure, but the software is largely unaffected
 | LOG_COMMAND - An activity related to processing commands has happened
 | LOG_ACTIVITY_HI - An important nominal event has happened
 | LOG_ACTIVITY_LO - An unimportant nominal event has happened that is a subset of an important event
 | LOG_DIAGNOSTIC - A detailed list of events in an activity that normally do not need to be seen; used for debugging
args | Serialized event arguments 


The `Fw::LogText` port has the following arguments:

Argument | Description
----- | -----------
id | Event identifier 
timeTag | The system time the event happened
severity | The severity of the event. 
 | TEXT_LOG_FATAL - A condition has been encountered that makes the software unable to continue
 | TEXT_LOG_WARNING_HI - A condition has been encountered that is a serious failure, but the software can continue
 | TEXT_LOG_WARNING_LO - A condition has been encountered that is a failure, but the software is largely unaffected
 | TEXT_LOG_COMMAND - An activity related to processing commands has happened
 | TEXT_LOG_ACTIVITY_HI - An important nominal event has happened
 | TEXT_LOG_ACTIVITY_LO - An unimportant nominal event has happened that is a subset of an important event
 | TEXT_LOG_DIAGNOSTIC - A detailed list of events in an activity that normally do not need to be seen; used for debugging
text | Text version of event 


#### 2.1.2 Serializables

##### 2.1.2.1 Fw::LogBuffer

The `Fw::LogBuffer` class represents a buffer to store a serialized parameter value.

## 3. Change Log

Date | Description
---- | -----------
9/3/2015 |  Initial Version

##### 2.1.2.1 Fw::LogStringArg

The `Fw::LogStringArg` class is used by the logging autocoder when string arguments are declared.

## 3. Change Log

Date | Description
---- | -----------
9/16/2015 |  Initial Version



