# Svc::TlmChan Component

## 1. Introduction

The `Svc::TlmChan` Component is used to store telemetry values written by other components. The values are stored in serialized form. 
The data is stored as a set of telemetry channels in a table. 
The data can be individually read back or periodically pushed to another component for transporting out of the system. 
`Svc::TlmChan` is an implementation of the `Svc::TlmStore` component in the `Svc/Tlm` directory.

## 2. Requirements

The requirements for `Svc::TlmChan` are as follows:

Requirement | Description | Verification Method
----------- | ----------- | -------------------
TLC-001 | The `Svc::TlmChan` component shall provide an interface to submit telemetry | Unit Test
TLC-002 | The `Svc::TlmChan` component shall provide an interface to read telemetry | Unit Test
TLC-003 | The `Svc::TlmChan` component shall provide an interface to run periodically to write telemetry | Unit Test
TLC-004 | The `Svc::TlmChan` component shall write changed telemetry channels when invoked by the run port | Unit Test
TLC-005 | The `Svc::TlmChan` component shall limit the number of telemetry entries processed per `Run` invocation to `TLMCHAN_MAX_ENTRIES_PER_RUN` | Unit Test
TLC-006 | The `Svc::TlmChan` component shall emit a `WARNING_HI` event when the per-run processing cap is reached | Unit Test

## 3. Design

### 3.1 Context

#### 3.1.1 Component Diagram

The `Svc::TlmChan` component has the following component diagram:

![Svc::TlmChan Diagram](img/TlmChanBDD.jpg "Svc::TlmChan")

#### 3.1.2 Ports

The `Svc::TlmChan` component uses the following port types:

Port Data Type | Name | Direction | Kind | Usage
-------------- | ---- | --------- | ---- | -----
[`Svc::Sched`](../../Sched/docs/sdd.md) | Run | Input | Asynchronous | Execute a cycle to write changed telemetry channels
[`Fw::Tlm`](../../../Fw/Tlm/docs/sdd.md) | TlmRecv | Input | Guarded Input | Update a telemetry channel
[`Fw::TlmGet`](../../../Fw/Tlm/docs/sdd.md) | TlmGet | Input | Guarded Input | Read a telemetry channel
[`Fw::Com`](../../../Fw/Com/docs/sdd.md) | PktSend | Output | n/a | Write a set of packets with updated telemetry
[`Svc::Ping`](../../Ping/docs/sdd.md) | pingIn | Input | Asynchronous | Receive a ping to verify liveness
[`Svc::Ping`](../../Ping/docs/sdd.md) | pingOut | Output | n/a | Return ping response
`Fw::Time` | timeCaller | Output | n/a | Retrieve current time for hash seed initialization
`Fw::Log` | eventOut | Output | n/a | Emit structured events (e.g. processing cap warning)
`Fw::LogText` | eventOutText | Output | n/a | Emit text-form events for ground display

#### 3.2 Functional Description

The `Svc::TlmChan` component has an input port `TlmRecv` that receives channel updates from other components in the system. These calls from the other components are made by the component implementation classes, but the generated code in the base classes takes the type specific channel value and serializes it, then makes the call to the output port. The `Svc::TlmChan` component can then store the channel value as generic data. The channel values are stored in an internal double-buffered table, and a flag is set when a new value is written to the channel entry.

When a request is made for a nonexistent channel, the call will return with an empty buffer in the Fw::TlmBuffer value argument. This is to cover the case where a channel is defined in the system, but has not been written yet. If the channel has not ever been defined, there is no way to programmatically determine that from the TlmGet port call.

The implementation uses a hashing function that is tuned in the configuration file `TlmChanImplCfg.hpp`. See section 3.5 for description.

#### 3.2.1 Run Handler Processing Cap

To bound the worst-case execution time of the `Run` handler and protect the rate group from telemetry bursts (caused by hardware anomalies, runaway components, software faults, or cyber-attacks), a per-invocation processing cap is enforced. The cap is configured via `TLMCHAN_MAX_ENTRIES_PER_RUN` in `TlmChanImplCfg.hpp`.

During each `Run` invocation:

- Up to `TLMCHAN_MAX_ENTRIES_PER_RUN` updated entries are serialized into downlink packets and sent via `PktSend`.
- Any updated entries beyond this limit are **deferred** (skipped) for the current cycle. Deferred entries are not re-queued; they are dropped when the active buffer is swapped at the start of the next `Run` invocation.
- If any entries are deferred, a `WARNING_HI` event `TlmChanEpochProcessingCapReached` is emitted via `eventOut` reporting the number of dropped entries and the cumulative count of invocations where the cap was reached.

The default value of `TLMCHAN_MAX_ENTRIES_PER_RUN` is `TLMCHAN_HASH_BUCKETS`, which makes the cap a no-op and preserves identical behavior to prior versions. Deployments that need to bound `Run` handler execution time can lower this value.

### 3.3 Scenarios

#### 3.3.1 External User Option

This diagram shows the scenario where telemetry packets are generated for an external user:

![External User Scenario](img/ExternalUserScenario.jpg) 

#### 3.3.2 Telemetry Database Option

This diagram shows the scenario where telemetry packets are stored and retrieved as a database:

![External User Scenario](img/DatabaseScenario.jpg) 

### 3.4 State

`Svc::TlmChan` has no state machines.

### 3.5 Algorithms

#### 3.5.1 Hash Function

In order to speed up lookups for storing and reading telemetry channels, a seeded hash function is used to select a location in an array of hash table slots. Configuration values in `TlmChanImplCfg.hpp` define the number of hash buckets and slots. The number of buckets must be at least as large as the number of telemetry channels defined in the system. The number of channels in the system can be determined by invoking `make comp_report_gen` from the deployment directory. `TLMCHAN_NUM_TLM_HASH_SLOTS` and `TLMCHAN_HASH_MOD_VALUE` can be varied to balance the amount of memory for slots versus the distribution of buckets to slots. See `TlmChanImplCfg.hpp` for tuning guidance.

The hash algorithm is selected based on the deployed size of `FwChanIdType`:

`FwChanIdType` size | Algorithm | Constants
------------------- | --------- | ---------
≥ 4 bytes (32-bit) | Murmur3 32-bit finalizer | `MURMUR3_C1 = 0x85EBCA6B`, `MURMUR3_C2 = 0xC2B2AE35`
2 bytes (16-bit) | Wang 16-bit hash | `WANG16_C1 = 0x2993`, `WANG16_C2 = 0xE877`
1 byte (8-bit) | XOR + modulo reduction | n/a

All three paths XOR the channel ID with a per-boot seed before hashing, then reduce the result modulo `TLMCHAN_NUM_TLM_HASH_SLOTS`.

#### 3.5.2 Hash Seed

The hash seed is computed once at construction time using `Os::RawTime` (OSAL-backed, no STL dependency) combined with stack-address entropy. The seed is stored in `m_hashSeed` and remains constant for the lifetime of the component. A non-zero seed is guaranteed; if the combined value is zero a known non-zero fallback constant is substituted.

Where the platform provides genuine boot-time entropy (a high-resolution clock that advances between boots and/or address-space randomization), the seed varies per boot, which raises the cost for an adversary attempting to craft channel IDs that collide to the same hash slot. On bare-metal targets without these properties (fixed boot-time clock value, deterministic memory layout) the seed may be effectively constant across boots; such deployments should supply a platform entropy source through the OSAL before relying on per-boot seed diversity as a security property.

### 3.6 Events

Event | Severity | Arguments | Description
----- | -------- | --------- | -----------
`TlmChanEpochProcessingCapReached` | WARNING_HI | `numDeferred: U32`, `numTimesDeferredCountReached: U32` | Emitted when `Run_handler` reaches `TLMCHAN_MAX_ENTRIES_PER_RUN` before processing all updated entries. `numDeferred` is the count of entries dropped this cycle; `numTimesDeferredCountReached` is the cumulative count of invocations where the cap was reached.

## 4. Dictionaries

TBD

## 5. Module Checklists

Checklist |
-------- |
[Design](Checklist_Design.xlsx) |
[Code](Checklist_Code.xlsx) |
[Unit Test](Checklist_Unit_Test.xls) |

## 6. Unit Testing

To see unit test coverage run fprime-util check --coverage

## 7. Change Log

Date | Description
---- | -----------
6/23/2015 | Design review edits
7/22/2015 | Design review actions
9/28/2015 | Unit Test Review additions
2026 | Added seeded Murmur3/Wang hash algorithm (replaces linear modulo); added `TLMCHAN_MAX_ENTRIES_PER_RUN` processing cap; added `TlmChanEpochProcessingCapReached` WARNING_HI event; added `timeCaller`, `eventOut`, `eventOutText`, `pingIn`, `pingOut` ports



