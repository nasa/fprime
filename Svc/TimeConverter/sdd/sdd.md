# Svc::TimeConverter Component

## 1. Introduction

`Svc::TimeConverter` converts `Fw::Time` objects between time bases. It owns a
fixed-capacity table of offsets between pairs of time bases, populated at runtime by a
project-specific correlation component over a port or by ground command. The component
knows nothing about concrete time-base values, so it remains project-agnostic: a project
that correlates its spacecraft clock against a workstation clock supplies the offset, and
the component applies it.

## 2. Requirements

| Requirement | Description | Verification |
|---|---|---|
| REQ-TIMECONVERTER-001 | `Svc::TimeConverter` shall store offsets between pairs of time bases, keyed by the pair in a canonical order so that one entry serves both directions. | Unit Test |
| REQ-TIMECONVERTER-002 | `Svc::TimeConverter` shall accept offset updates at runtime from a connected component, adding new pairs and replacing the offset of pairs already stored. | Unit Test |
| REQ-TIMECONVERTER-003 | `Svc::TimeConverter` shall store at most a compile-time configurable number of offset entries. | Unit Test |
| REQ-TIMECONVERTER-004 | `Svc::TimeConverter` shall emit a warning event and discard the update when an offset update would exceed the configured table capacity. | Unit Test |
| REQ-TIMECONVERTER-005 | `Svc::TimeConverter` shall convert a supplied time to a requested time base by applying the stored offset for that pair, negating the stored offset when the conversion runs opposite to the stored direction, and shall report success. | Unit Test |
| REQ-TIMECONVERTER-006 | `Svc::TimeConverter` shall preserve the time context of the input time in the converted output time. | Unit Test |
| REQ-TIMECONVERTER-007 | `Svc::TimeConverter` shall report success without altering the value when the source and requested time bases are identical. | Unit Test |
| REQ-TIMECONVERTER-008 | `Svc::TimeConverter` shall report `UNKNOWN_TIMEBASE`, emit a throttled warning event, and leave the output unmodified when no offset is stored for the requested pair. | Unit Test |
| REQ-TIMECONVERTER-009 | `Svc::TimeConverter` shall report `INVALID_TIME`, emit a throttled warning event, and leave the output unmodified when the conversion would produce a time not representable as an `Fw::Time`. | Unit Test |
| REQ-TIMECONVERTER-010 | `Svc::TimeConverter` shall accept a command that sets the offset for a pair of time bases, with the same effect as an offset update received over the port, shall report the stored offset as an event, and shall reject the command with a warning event when an argument is rejected. | Unit Test |
| REQ-TIMECONVERTER-011 | `Svc::TimeConverter` shall accept a command that discards every stored offset and reports the number discarded as an event. | Unit Test |
| REQ-TIMECONVERTER-012 | `Svc::TimeConverter` shall accept a command that reports the offset stored for a single pair of time bases as an event, and shall reject the command with a warning event when no offset is stored for that pair or when both ends of the pair are the same time base. | Unit Test |
| REQ-TIMECONVERTER-013 | `Svc::TimeConverter` shall accept a command that reports every stored offset as events, and shall emit an event reporting an empty table when no offsets are stored. | Unit Test |
| REQ-TIMECONVERTER-014 | `Svc::TimeConverter` shall perform conversions and offset updates without dynamic memory allocation and in bounded time. | Inspection |
| REQ-TIMECONVERTER-015 | `Svc::TimeConverter` shall reject a time base that denotes no clock, `TB_NONE` or `TB_DONT_CARE`, wherever it is supplied as an end of a pair, and shall emit a throttled high-severity warning event naming it. | Unit Test |

## 3. Design

`Svc::TimeConverter` is a passive component: conversions execute in the caller's thread.
The offset table is shared between the conversion port, the update port, and the
commands, so all of them are guarded.

### 3.1 Offset table

An entry maps a pair of time bases, held as an `Svc.TimeBasePair`, to the offset in
microseconds between them. Each unordered pair occupies exactly one entry, keyed in a
canonical order — the time base with the lesser numeric value first — and the stored
offset is the value added to a time in the lesser base to obtain a time in the greater
base. A conversion running in the opposite direction negates the stored offset. Supplying
the same pair again replaces the existing entry regardless of the order the pair is
supplied in.

The table holds `Svc::TimeConverterCfg::MAX_OFFSET_ENTRIES` entries in a fixed-capacity
`Fw::ArrayMap`, so no memory is allocated after construction and every operation is
bounded by the table size. An update naming a pair that is not stored while the table is
full is discarded and reported with `OffsetTableFull`; replacing the offset of a pair
already stored still succeeds when the table is full.

The capacity is intended to stay small. Every command handler holds the component guard
while it emits its events and its command response, so a conversion arriving on another
thread blocks for that interval; `DUMP_OFFSETS` is the longest of them, emitting one
event per entry.

A deployment must not route this component's `timeCaller` or `eventOut` through a
component that converts time through it: the guard is not recursive, so such a cycle
would deadlock the calling thread.

### 3.2 Conversion

The caller pre-sets the time base of the `out_time` argument to the base it wants; the
component fills in the seconds and microseconds on success. The time context of
`in_time` is copied into `out_time`, since a conversion changes the time base but not the
context the time was taken in.

Conversions are single-hop: a pair with no stored offset fails even when the two bases
are reachable through an intermediate base.

| Condition | Status | Output |
|---|---|---|
| Either base denotes no clock | `UNKNOWN_TIMEBASE` | unmodified |
| Source and requested bases are identical | `OK` | input time copied unchanged |
| Offset stored for the pair, result representable | `OK` | converted time |
| No offset stored for the pair | `UNKNOWN_TIMEBASE` | unmodified |
| Result negative or beyond the `Fw::Time` range | `INVALID_TIME` | unmodified |

Offsets are range-checked when stored, bounded to the largest time an `Fw::Time` can
represent, so applying or negating a stored offset cannot overflow.

### 3.2.1 Time bases that denote no clock

`TB_NONE` records that no time base has been established and `TB_DONT_CARE` is a
sequencer wildcard; neither names a clock an offset could relate to another. The
component rejects both wherever they appear as an end of a pair — conversion, offset
updates over the port, `SET_OFFSET`, and `GET_OFFSET` — emitting `UnusableTimeBase` for
each end supplied that way. A project that replaces the `TimeBase` enumeration keeps
this behavior, as both values are required members of it.

### 3.3 Ports

| Port | Kind | Data Type | Description |
|---|---|---|---|
| `convertTime` | guarded input | `Svc.ConvertTime` | Converts a time into the base carried by `out_time` |
| `offsetUpdate` | guarded input | `Svc.TimeOffsetSend` | Receives an offset between a pair of time bases |

`offsetUpdate` emits no event on success: a correlation component is expected to update
offsets continuously, which would otherwise flood the event stream. Rejected updates are
reported.

### 3.4 Commands

| Command | Description | Failure |
|---|---|---|
| `SET_OFFSET` | Stores the offset for a pair of time bases and reports it with `OffsetSet` | `VALIDATION_ERROR` with `UnusableTimeBase`, `IdenticalTimeBases`, or `OffsetOutOfRange` for a rejected argument; `EXECUTION_ERROR` with `OffsetTableFull` when the table is full |
| `CLEAR_OFFSETS` | Discards every stored offset and reports the number discarded | none |
| `GET_OFFSET` | Reports the offset stored for one pair with `OffsetReport` | `VALIDATION_ERROR` with `UnusableTimeBase` or `IdenticalTimeBases`; `EXECUTION_ERROR` with `NoOffsetStored` when the pair is not stored |
| `DUMP_OFFSETS` | Reports every stored offset with `OffsetReport`, one event per entry with the lesser time base first, in the order the pairs were first stored, or `OffsetTableEmpty` | none |

A full table is an operator-clearable state rather than a bad argument, which is why
`SET_OFFSET` distinguishes it with `EXECUTION_ERROR`.

`CLEAR_OFFSETS` also resets the throttle counters of the six throttled events, so those
warnings resume after the operator has acted on them.

### 3.5 Events

| Event | Severity | Throttle | Description |
|---|---|---|---|
| `NoConversionAvailable` | warning low | 5 | No offset is stored for a requested conversion |
| `InvalidTime` | warning low | 5 | A conversion result is not representable as an `Fw::Time` |
| `OffsetSet` | activity high | none | An offset was stored by command |
| `OffsetTableFull` | warning high | 5 | An offset was discarded because the table is full |
| `OffsetsCleared` | activity high | none | Every stored offset was discarded |
| `OffsetReport` | activity high | none | Report of a stored offset |
| `NoOffsetStored` | warning low | none | No offset is stored for a requested pair |
| `OffsetTableEmpty` | activity high | none | The table holds no entries |
| `IdenticalTimeBases` | warning low | 5 | A single time base was supplied as both ends of a pair |
| `OffsetOutOfRange` | warning high | 5 | An offset exceeds the representable time range |
| `UnusableTimeBase` | warning high | 5 | A time base denoting no clock was supplied as an end of a pair |

## 4. Configuration

`Svc::TimeConverterCfg::MAX_OFFSET_ENTRIES` in
`Svc/TimeConverter/TimeConverterConfig/TimeConverterCfg.fpp` sets the table
capacity; projects override it. The default of 2 suits the three convertible time bases
of the default configuration; a project should raise it to the number of pairs it
correlates, keeping it small enough that a `DUMP_OFFSETS` remains a short operation.

The constant lives in a component-local configuration module rather than
`default/config`, so a project overrides it by listing its own `TimeConverterCfg.fpp`
under `CONFIGURATION_OVERRIDES` in a project configuration module, as described under
"Library Default Configuration" in
[configuring F Prime](../../../docs/user-manual/framework/configuring-fprime.md).

## 5. Change Log

Date | Description
---- | -----------
9/13/2026 | Initial version
