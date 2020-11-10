\page FwTlmPort Fw::Tlm Port
# Fw::Tlm Port

## 1. Introduction

The `Fw::Tlm` port is used to pass a serialized telemetry value. It also passes the telemetry channel IS as well a time tag representing the time the channel was written. 

## 2. Design

### 2.1 Context

#### 2.1.1 Port Diagram

The `Fw::Tlm` port has the following port diagram:

![Fw::Com Diagram](img/FwTlmBDD.jpg "Fw::Com Port")

#### 2.1.2 Serializables

##### 2.1.2.1 Fw::ComPacket 

The `Fw::TlmPacket` class is a packet class derived from [`Fw::ComPacket`](../../Com/docs/sdd.html) that provides methods for encoding a telemetry packet. 

##### 2.1.2.2 Fw::ComBuffer

The `Fw::TlmBuffer` class contains a buffer that holds the serialized value of a telemetry channel. This buffer is passed as an argument to the `Fw::Tlm` port.

## 3. Change Log

Date | Description
---- | -----------
6/23/2015 |  Initial Version
