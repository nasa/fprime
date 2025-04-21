# Svc::OsTime Component

## 1. Introduction

The `Svc::OsTime` is a component that provides system time on any system that implements the RawTime OSAL class. The RawTime interface provides Fw::TimeInterval objects.

## 2. Requirements

| Requirement     | Description                                                                                                        | Verification |
|-----------------|--------------------------------------------------------------------------------------------------------------------|--------------|
| SVC-OS-TIME-001 | `Svc::OsTime` shall return current system time as an `Fw::Time` objects in response to the `timeGetPort` port call | Unit Test    |
| SVC-OS-TIME-002 | `Svc::OsTime` shall return ZERO_TIME as an `Fw::Time` object if the RawTime layer returns an error code            | Unit Test    |

## 3. Design
`Svc::OsTime` has two ports, one `timeGetPort` for the F Prime Time interface and a `setEpoch` port which is described later. It has no data types, commands, events, or telemetry channels.

`Svc::OsTime` uses an epoch time provided during initialization to calculate the absolute system time. An epoch consists of both an `Fw::Time` object and a `Os::RawTime` object that represent the same moment in time. `Svc::OsTime` calculates the time since the epoch using an `Os::RawTime` at the current time and the provided epoch time. The time since epoch is added to the `Fw::Time` timestamp to form a new `Fw::Time` object representing the current time. The TimeBase and TimeContext for the return `Fw::Time` object match the corresponding epoch `Fw::Time`.

If at any point an error status is returned from the RawTime implementation then a ZERO_TIME value is returned from the port call

## 3.1 Setup

The epoch timestamp pair should be set prior to using the component. Prior to passing in the epoch timestamp, OsTime will return ZERO_TIME to all port requests.

The epoch timestamp pair can be set by either directly call the `set_epoch` method, or by calling the `setEpoch` port call on the component. It is expected that `set_epoch` will be called during topology setup and the `setEpoch` port will be used during runtime, but this is not required.

The epoch timestamp pair may be changed after initial operation to update the epoch time or the TimeBase/TimeContext for the returned `Fw::Time` object.

### 3.2 Ports

| Port          | Kind       | Data Type         | Description                          |
|---------------|------------|-------------------|--------------------------------------|
| `timeGetPort` | sync input | `Fw.Time`         | Port returning current system design |
| `setEpoch`    | sync input | `Svc.OsTimeEpoch` | Port to set an epoch timestamp pair  |

## 7. Change Log

Date | Description
---- | -----------
3/10/2025  | Initial Version

