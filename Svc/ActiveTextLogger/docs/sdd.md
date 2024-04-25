\page SvcActiveTextLoggerComponent Svc::ActiveTextLogger Component
# Svc::ActiveTextLogger Component

## 1. Introduction

The `Svc::ActiveTextLogger` component processes log texts from other components. The logs are written to standard output and optionally to a file.

## 2. Requirements

The requirements for `Svc::ActiveTextLogger` are as follows:

Requirement | Description | Verification Method
----------- | ----------- | -------------------
ISF-ATL-001 | The `Svc::ActiveTextLogger` component shall print received log texts to standard output. | Unit Test; Inspection
ISF-ATL-002 | The `Svc::ActiveTextLogger` component shall write received log texts to an optionally supplied file. | Unit Test
ISF-ATL-003 | The `Svc::ActiveTextLogger` component shall format the passed log text on the calling thread context, and perform all other processing on the component's thread. | Inspection
ISF-ATL-004 | The `Svc::ActiveTextLogger` component shall stop writing to the optional file if it would exceed its max size. | Unit Test
ISF-ATL-005 | The `Svc::ActiveTextLogger` component shall provide a public method to supply the filename to write to and max size. | Unit Test
ISF-ATL-006 | The `Svc::ActiveTextLogger` component shall attempt to create a new file to write to if the supplied one already exists.  It will try up to ten times, by adding an integer suffix to the filename, ie "file","file0","file1"..."file9" | Unit Test


## 3. Design

### 3.1 Context

#### 3.1.1 Component Diagram

The `Svc::ActiveTextLogger` component has the following component diagram:

> TODO

#### 3.1.2 Ports

The `Svc::ActiveTextLogger` component uses the following port types:

Port Data Type | Name | Direction | Kind | Usage
-------------- | ---- | --------- | ---- | -----
[`Fw::LogText`](../../../Fw/Log/docs/sdd.md) | TextLogger | Input | Synchronous | Logging port

### 3.2 Functional Description

The `Svc::ActiveTextLogger` component provides a text logging function for the software that maintains consistent ordering by writing the logs from a thread.

#### 3.2.1 File Writing

Once a valid file and max size is supplied via a public function call, the `Svc::ActiveTextLogger` component writes to that file as well as standard output.  The `Svc::ActiveTextLogger` component will stop writing to the file if it would exceed the maximum size.

If the file supplied already exists, the `Svc::ActiveTextLogger` component will attempt to create a new file up to ten times by appending an integer suffix to end of the file name.

### 3.3 Scenarios

TODO

### 3.4 State

`Svc::ActiveTextLogger` has no state machines.

### 3.5 Algorithms

`Svc::ActiveTextLogger` has no significant algorithms.

## 4. Dictionaries

## 5. Module Checklists

## 6. Unit Testing

To see unit test coverage run fprime-util check --coverage

## 7. Change Log

Date | Description
---- | -----------
5/11/2017 | Initial SDD



