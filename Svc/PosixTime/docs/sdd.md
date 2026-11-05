# Svc::PosixTime Component

## 1. Introduction

The `Svc::PosixTime` is a component that provides system time on Posix systems. It implements the Time interface.

## 2. Requirements

| Requirement        | Description                                                                                                           | Verification |
|--------------------|-----------------------------------------------------------------------------------------------------------------------|--------------|
| SVC-POSIX-TIME-001 | `Svc::PosixTime` shall return current system time as an `Fw::Time` objects in response to the `timeGetPort` port call | Unit Test    |
| SVC-POSIX-TIME-002 | `Svc::PosixTime` shall set the time context of returned `Fw::Time` objects to the value supplied by `setTimeContext`  | Unit Test    |

## 3. Design

`Svc::PosixTime` has a single port. It has no data types, commands, events, telemetry channels, nor substantial algorithms.

### 3.1 Ports

| Port          | Kind       | Data Type | Description                          |
|---------------|------------|-----------|--------------------------------------|
| `timeGetPort` | sync input | `Fw.Time` | Port returning current system design |

### 3.2 Time Context

Returned `Fw::Time` objects carry a project-specific time context, which is `0` unless changed by a call to `setTimeContext()`. `setTimeContext()` is not synchronized and is intended to be called at startup, before the component is serving `timeGetPort` calls.

## 7. Change Log

Date | Description
---- | -----------
4/20/2017  | Initial Version
10/12/2023 | Reworked into `Svc::PosixTime` 
8/14/2026  | Added settable time context
