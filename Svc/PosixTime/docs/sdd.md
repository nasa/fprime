\page SvcPosixTime Svc::PosixTime Component
# Svc::PosixTime Component

## 1. Introduction

The `Svc::PosixTime` is a component that provides system time on Posix systems. It implements the Time interface.

## 2. Requirements

| Requirement        | Description                                                                                                           | Verification |
|--------------------|-----------------------------------------------------------------------------------------------------------------------|--------------|
| SVC-POSIX-TIME-001 | `Svc::PosixTime` shall return current system time as an `Fw::Time` objects in response to the `timeGetPort` port call | Unit Test    |

## 3. Design

`Svc::PosixTime` has a single port. It has no data types, commands, events, telemetry channels, nor substantial algorithms.

### 3.1 Ports

| Port          | Kind       | Data Type | Description                          |
|---------------|------------|-----------|--------------------------------------|
| `timeGetPort` | sync input | `Fw.Time` | Port returning current system design |

## 7. Change Log

Date | Description
---- | -----------
4/20/2017  | Initial Version
10/12/2023 | Reworked into `Svc::PosixTime` 

