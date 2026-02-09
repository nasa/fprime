# Ccsds::CfdpManager

## What is CFDP?

The CCSDS File Delivery Protocol (CFDP) is a space communication standard designed for reliable, autonomous file transfer in space missions. CFDP provides a robust mechanism for transferring files between ground systems and spacecraft even in environments with long propagation delays, intermittent connectivity, or high error rates.

CFDP is particularly well-suited for:
- Spacecraft-to-ground file transfers: Downlinking F' event logs, telemetry data files, science data products, and diagnostic files
- Ground-to-spacecraft file transfers: Uplinking F' flight software updates, parameter files, and command sequences
- Delay-tolerant and disruption-tolerant delivery: Automatic retry and recovery mechanisms for challenging communication links

The protocol supports two operational modes:
- Class 1 (Unacknowledged): Unreliable transfer with no acknowledgments, suitable for real-time or non-critical data where speed is prioritized
- Class 2 (Acknowledged): Reliable transfer with acknowledgments, retransmissions, and gap detection, ensuring complete and verified file delivery

For complete protocol details, refer to the [CCSDS 727.0-B-5 - CCSDS File Delivery Protocol (CFDP)](https://ccsds.org/Pubs/727x0b5e1.pdf) Blue Book specification.

## CFDP as an F' Component

The CfdpManager component provides an F' implementation of the CFDP protocol. Substantial portions of this implementation were ported from [NASA's CF (CFDP) Application in the Core Flight System (cFS) version 3.0.0](https://github.com/nasa/CF/releases/tag/v3.0.0). The ported code includes:
- Core CFDP engine and transaction management logic
- Protocol state machines for transmit and receive operations
- Utility functions for file handling and resource management
- Chunk and gap tracking for Class 2 transfers

The F' implementation adds new components built specifically for the F' ecosystem:
- CfdpManager component wrapper: Integrates CFDP into F' architecture with standard port interfaces, commands, events, telemetry, and parameters
- Object-oriented PDU encoding/decoding: Type-safe PDU classes based on F' `Serializable` interface for consistent serialization
- F' timer implementation: Uses F' time primitives for protocol timers

For detailed attribution, licensing information, and a complete breakdown of ported vs. new code, see [ATTRIBUTION.md](../ATTRIBUTION.md).

## Usage Examples
Add usage examples here

### Diagrams
Add diagrams here

### Typical Usage
And the typical usage of the component here

## Class Diagram
Add a class diagram here

## Port Descriptions
| Name | Description |
|---|---|
|---|---|

## Component States
Add component states in the chart below
| Name | Description |
|---|---|
|---|---|

## Sequence Diagrams
Add sequence diagrams here

## Parameters
| Name | Description |
|---|---|
|---|---|

## Commands
| Name | Description |
|---|---|
|---|---|

## Events
| Name | Description |
|---|---|
|---|---|

## Telemetry
| Name | Description |
|---|---|
|---|---|

## Unit Tests
Add unit test descriptions in the chart below
| Name | Description | Output | Coverage |
|---|---|---|---|
|---|---|---|---|

## Requirements
Add requirements in the chart below
| Name | Description | Validation |
|---|---|---|
|---|---|---|

## Change Log
| Date | Description |
|---|---|
|---| Initial Draft |