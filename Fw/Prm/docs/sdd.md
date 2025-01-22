# Fw::PrmGet / PrmSet Ports

## 1. Introduction

The `Fw::PrmGet` port is used to retrieve a parameter value from storage. The parameter ID and a target buffer for the value is passed as arguments, and a status indicating the validity of the value is returned.

The `Fw::PrmSet` port is used to update parameter values. The parameter ID and a buffer with the value is passed as arguments.

## 2. Design

### 2.1 Context

#### 2.1.1 Port Diagram

The `Fw::PrmGet` port has the following port diagram:

![`Fw::PrmGet` Diagram](img/PrmGetBDD.jpg "Fw::PrmGet Port")

The `Fw::PrmGet` port has the following return values:

Value | Description
----- | -----------
PARAM_UNINIT | Used only in component; indicates parameter hasn't been initialized yet, i.e. loadParameters() call was never made 
PARAM_VALID | Parameter was successfully retrieved from storage; returned by port call and used in component
PARAM_INVALID | Parameter was not successfully retrieved from storage; returned by port call and used in component
PARAM_DEFAULT | Default value was used for parameter if specified. Used only in component to indicate PARAM_INVALID was returned by port so default was needed

The `Fw::PrmSet` port has the following port diagram:

![`Fw::PrmSet` Diagram](img/PrmSetBDD.jpg "Fw::PrmSet Port")

#### 2.1.2 Serializables

##### 2.1.2.1 Fw::PrmBuffer

The `Fw::PrmBuffer` class represents a buffer to store a serialized parameter value.

## 3. Change Log

Date | Description
---- | -----------
6/23/2015 |  Initial Version



