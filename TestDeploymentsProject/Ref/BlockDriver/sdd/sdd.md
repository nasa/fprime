# Ref::BlockDriver Component

## 1. Introduction

The `Ref::BlockDriver` is a demonstration component that loops back data buffers. It also emulates a timing interrupt driven by the public method `void callIsr(void)`.

## 2. Requirements

The requirements for `Ref::BlockDriver` are as follows:

Requirement | Description | Verification Method
----------- | ----------- | -------------------
ISF-BDV-001 | The `Ref::BlockDriver` component shall loop back packets from its input port to its output port | System test, unit test
ISF-BDV-002 | The `Ref::BlockDriver` component shall loop back the key value from  `PingIn` port to `PingOut` port | System test, unit test

## 3. Design

### 3.1 Context

#### 3.1.1 Component Diagram

The `Ref::BlockDriver` component has the following component diagram:

![`Ref::BlockDriver` Diagram](img/BlockDriverBDD.jpg "Ref::BlockDriver")

## 4. Dictionaries

TBD

## 5. Module Checklists

## 6. Unit Testing

## 7. Change Log

Date | Description
---- | -----------
4/20/2017 | Initial Version
5/11/2025 | Add requirement for ping key loopback
6/4/2025 | Move from `Drv` to `Ref`



