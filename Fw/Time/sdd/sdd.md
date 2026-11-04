# Fw::Time Port

## 1. Introduction

The `Fw::Time` port is used to pass time stamps around the system, either for telemetry and events or other uses.
The `Fw::TimeInterval` port is used to pass time intervals.

## 2. Design

### 2.1 Context

#### 2.1.1 Port Diagram

The `Fw::Time` port has the following port diagram:

![Fw::Time Diagram](img/TimeBDD.jpg "Fw::Time Port")

#### 2.1.2 Serializables

The following serializables are provided:

##### 2.1.2.1 Fw::Time

`Fw::Time` is a class that stores:  
    1. the time base (project specific)  
    2. the time context  
    3. seconds portion of the time stamp  
    4. microseconds portion of the time stamp  

Note that the representation of the time value is defined by the FPP TimeValue data structure (in Time.fpp)
while the port itself uses a `Fw::Time` object.

##### 2.1.2.1 Fw::TimeInterval

`Fw::TimeInterval` is a class that stores:  
    1. seconds portion of a time interval  
    2. microseconds portion of a time interval  

Note that the representation of the time interval value is defined by the FPP TimeIntervalValue data structure (in
Time.fpp) while the port itself uses a `Fw::TimeInterval` object.

## 3. Change Log

Date | Description
---- | -----------
6/19/2015 |  Initial Version
7/16/2025 |  Update Serializables description


