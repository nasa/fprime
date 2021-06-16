\page SvcPolyDbComponent Svc::PolyDb Component
# Svc::PolyDb Component

## 1. Introduction

The `Svc::PolyDb` component is meant to act as a resident database of values that components can use for checking or for data in algorithms. 

## 2. Requirements

The requirements for `Svc::PolyDb` are as follows:

Requirement | Description | Verification Method
----------- | ----------- | -------------------
PDB-001 | The `Svc::PolyDb` component shall store `Fw::PolyType` values. | Inspection, Unit Test
PDB-002 | The `Svc::PolyDb` component shall allow the `Fw::PolyType` values to be read and written. | Unit Test 
PDB-003 | The `Svc::PolyDb` component shall time tag the data | Unit Test 
PDB-004 | The `Svc::PolyDb` component shall report the measurement state of the data (good, stale, failure) | Unit Test 

## 3. Design

### 3.1 Context

#### 3.1.1 Component Diagram

The `Svc::PolyDb` component has the following component diagram:

![`Svc::PolyDb` Diagram](img/PolyDbBDD.jpg "Svc::PolyDb")

#### 3.1.2 Ports

The Svc::RateGroupDriver component uses the following port types:

Port Data Type | Name | Direction | Kind | Usage
-------------- | ---- | --------- | ---- | -----
[`Svc::Poly`](../../PolyIf/docs/sdd.html) | getValue | Input | Guarded | Write `Fw::PolyType` values
[`Svc::Poly`](../../PolyIf/docs/sdd.html) | setValue | Input | Guarded | Write `Fw::PolyType` values

#### 3.2 Functional Description

`Fw::PolyType` is different from binary telemetry in that it is not in a serialized form, but is stored as the native type. 
The component stores a table of `Fw::PolyType' objects which are read and written by table index. 
The table is protected by a mutex to prevent simultaneous access.

### 3.3 Scenarios

#### 3.3.1 Read and Write Values

As described in the Functional Description section, the RateGroupDriver component accepts calls to the SchedIn and divides them down to the SchedOut ports:

![Read and Write Values](img/PolyDbReadWriteScenario.jpg) 

### 3.4 State

`Svc::PolyDb` has no state machines.

### 3.5 Algorithms

`Svc::PolyDb` has no significant algorithms.

## 4. Dictionaries

No dictionaries

## 4. Module Checklists

Document | Link
-------- | ----
Design Checklist | [Link](Checklist_Design.xlsx)
Code Checklist | [Link](Checklist_Design.xlsx)
Unit Test Checklist | [Link](Checklist_Unit_Test.xlsx)

## 5. Unit Testing

To see unit test coverage run fprime-util check --coverage

## 6. Change Log

Date | Description
---- | -----------
6/19/2015 | Design review edits
7/22/2015 | Design review actions 
9/15.2015 | Unit Test actions



