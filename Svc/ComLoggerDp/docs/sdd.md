# Svc::ComLoggerDp

## 1. Introduction

The ComLoggerDp component logs `Fw::ComBuffer` buffers (e.g., framed telemetry, events, or command packets) to F Prime Data Product records. The component can be commanded to start recording data products, stop recording data product, or modify the priority of existing data products in progress. This component is meant to replace the `ComLogger` in deployments where data product management of `Fw::ComBuffers` is desired.

## 2. Requirements

| Name | Description | Validation |
|---|---|---|
| SVC-COMLOGGERDP-001 | The ComLoggerDp component shall log the contents of Com buffers received on its `comIn` port | unit test |
| SVC-COMLOGGER-002 | The ComLoggerDp component shall have a command to start recording packets, specifying the number of packets per container|
| SVC-COMLOGGER-003 | The ComLoggerDp component shall have a command to stop recording packets|
| SVC-COMLOGGER-004 | The ComLoggerDp component shall have a command to modify the priority of existing data products|
| SVC-COMLOGGER-005 | If the provided container buffer is not large enough to fit the requested number of records per container, emit a WARNING_LO event and adjust to the smaller size. The event should have a throttle value defined in an FPP configuration file with adefault of 1. Increment a DpBufferOverflow counter|
| SVC-COMLOGGER-006 | A public `configure` function will specify whether data product logging is initially enabled|


## 3. Design

The ComLogger is an active component. Com buffers arriving on the async `comIn` port are dispatched on the component's thread and written synchronously to data product records.

### 3.1 Port Description

### 3.3 Initialization

### 3.4 Commands

| Command | Description |
|---|---|

### 3.5 Events

| Event | Severity | Description |
|---|---|---|

## 4. Usage


## 5. Change Log

| Date | Description |
|---|---|
