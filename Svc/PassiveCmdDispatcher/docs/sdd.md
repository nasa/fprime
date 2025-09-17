# Svc::PassiveCmdDispatcher

A passive component for dispatching commands. Designed as a pared-down, passive replacement for
[`Svc::CmdDispatcher`](../../CmdDispatcher/docs/sdd.md).

## Usage Examples
Add usage examples here

### Diagrams
Add diagrams here

### Typical Usage
And the typical usage of the component here

## Class Diagram
Add a class diagram here

## Port Descriptions
| Port Type | Name | Kind | Description |
|---|---|---|---|
| [`Fw::CmdReg`](../../../Fw/Cmd/docs/sdd.md) | `compCmdReg` | `sync input` | Command registration for components |
| [`Fw::Cmd`](../../../Fw/Cmd/docs/sdd.md) | `compCmdSend` | `output` | Send commands to components |
| [`Fw::CmdResponse`](../../../Fw/Cmd/docs/sdd.md) | `compCmdStat` | `sync input` | Port for components to report command status |
| [`Fw::Com`](../../../Fw/Com/docs/sdd.md) | `seqCmdBuff` | `sync input` | Receive command buffer |
| [`Fw::CmdResponse`](../../../Fw/Cmd/docs/sdd.md) | `seqCmdStatus` | `output` | Send command status to command buffer source |

## Component States
`Svc::PassiveCmdDispatcher` has no state machines.

## Sequence Diagrams
Add sequence diagrams here

## Parameters
None.

## Commands
| Name | Description |
|---|---|
| `CMD_NO_OP` | Basic no-op command |
| `CMD_CLEAR_TRACKING` | Clear any command tracking status |

## Events
| Name | Description |
|---|---|
| `OpCodeDispatched` | Command dispatched |
| `OpCodeCompleted` | Command completed successfully |
| `OpCodeError` | Command completed with failure |
| `MalformedCommand` | Received a malformed command packet |
| `InvalidCommand` | Received an invalid opcode |
| `TooManyCommands` | Received a command when the sequence tracker table was already full |
| `NoOpReceived` | Output when a `CMD_NO_OP` is received |

## Telemetry
None.

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
| 2025-09-16 | Initial Draft |
