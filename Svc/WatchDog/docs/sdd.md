# Svc::WatchDog Port

## 1. Introduction

The `Svc::WatchDog` port is used to ping active components to verify that they are still responsive and have not hung.  

## 2. Design

A health component in the system uses a set of ping ports to send a message to an active component. The active component is required to respond with the value in the key argument passed in the port by a certain timeout.

### 2.1 Context

#### 2.1.1 Port Diagram

The `Svc::WatchDog` port has the following port diagram:

![`Svc::WatchDog` Diagram](img/WatchDogBDD.jpg "Svc::WatchDog Port")

The `Svc::WatchDog` port has the following arguments:

Argument | Type | Description
----- | -----------
code | U32 | Code for the watchdog reset  

#### 2.1.2 Serializables

There are no serializables defined in the port

## 3. Change Log

Date | Description
---- | -----------
1/25/2016 |  Initial Version
