\page SvcPolyPort Svc::Poly Port
# Svc::Poly Port

## 1. Introduction

The `Svc::Poly` is used to pass the polymorphic `Fw::PolyType` between components. 
See the [`Svc::PolyType`](../../../Fw/Types/docs/sdd.md) description for details on the class.

The arguments are as follows:

Argument|Description|Values
--------|-----------|------
entry   | The index of the entry in the table
status  | The status of the measurement  | `MEASUREMENT_OK` = normal measurement, `MEASUREMENT_FAILURE` = error reading measurement, `MEASUREMENT_STALE` = measurement out of date
time    | The time tag of the measurement
val     | The value of the measurement

## 2. Design

### 2.1 Context

#### 2.1.1 Port Diagram

The `Svc::Poly` port has the following port diagram:

![`Svc::Poly` Diagram](img/PolyIfBDD.jpg "Svc::Poly Port")

## 3. Change Log

Date | Description
---- | -----------
6/24/2015 |  Initial Version
1/7/2016 | Added BDD diagram



