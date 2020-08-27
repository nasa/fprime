# Svc::Ping Port

## 1. Introduction

The `Svc::Ping` port is used to ping active components to verify that they are still responsive and have not hung.  

## 2. Design

A health component in the system uses a set of ping ports to send a message to an active component. The active component is required to respond with the value in the key argument passed in the port by a certain timeout.

### 2.1 Context

#### 2.1.1 Port Diagram

The `Svc::Ping` port has the following port diagram:

![`Svc::Ping` Diagram](img/PingBDD.jpg "Svc::Ping Port")

The `Svc::Ping` port has the following arguments:

Argument | Type | Description
----- | -----------
key | U32 | Key that the active component is required to return  

#### 2.1.2 Serializables

There are no serializables defined in the port

## 3. Change Log

Date | Description
---- | -----------
1/5/2016 |  Initial Version
