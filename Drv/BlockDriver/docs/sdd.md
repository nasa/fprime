# Drv::BlockDriver Component

## 1. Introduction

The `Drv::BlockDriver` is a demonstration component that loops back data buffers. It also emulates a timing interrupt driven by the public method `void callIsr(void)`.

## 2. Requirements

The requirements for `Drv::BlockDriver` are as follows:

Requirement | Description | Verification Method
----------- | ----------- | -------------------
ISF-BDV-001 | The `Drv::BlockDriver` component shall loop back packets from its input port to its output port | System test
ISF-BDV-002 | The `Drv::BlockDriver` component shall send a timing interrupt whenever `callIsr()` is invoked | System test

## 3. Design

### 3.1 Context

#### 3.1.1 Component Diagram

The `Drv::BlockDriver` component has the following component diagram:

![`Drv::BlockDriver` Diagram](img/BlockDriverBDD.jpg "Drv::BlockDriver")

## 4. Dictionaries

TBD

## 5. Module Checklists

## 6. Unit Testing

## 7. Change Log

Date | Description
---- | -----------
4/20/2017 | Initial Version



