# Svc::PassThroughRouter

The `Svc::PassThroughRouter` component is a direct pass through router for single custom route use cases.

The `Svc::PassThroughRouter` component receives F´ packets (as [Fw::Buffer](../../../Fw/Buffer/docs/sdd.md) objects) and passes them through to other components through synchronous port calls. The input port of type `Svc.ComDataWithContext` passes this Fw.Buffer object along with optional context data which is typically used for routing. The `Svc::PassThroughRouter` is intended to be used in cases where there is only a single route. The `Svc::PassThroughRouter` ignores the received context data and passes the Fw.Buffer through on the `allPacketsOut` port.

All buffers sent by `Svc::PassThroughRouter` on the `allPacketsOut` port are expected to be returned through the `allPacketsReturnIn` port. To satisfy the `dataReturnOut` port interface, the default context value is used when returning the buffer.

## Port Descriptions

| Kind | Name | Type | Description |
|---|---|---|---|
| `sync input` | `dataIn` | `Svc.ComDataWithContext` | Receiving Fw::Buffer with context buffer |
| `output` | `dataReturnOut` | `Svc.ComDataWithContext` | Returning ownership of buffer received on `dataIn` |
| `output` | `allPacketsOut` | `Fw.BufferSend` | Pass-through port for sending all packets received on `dataIn` |
| `sync input` | `allPacketsReturnIn` | `Fw.BufferSend` | Receiving back ownership of buffer sent on `allPacketsOut` |

## Requirements
Add requirements in the chart below
| Name | Description | Validation |
|---|---|---|
| SVC-PASS_THROUGH_ROUTER-001 | `Svc::PassThroughRouter` shall route packets received on `dataIn` to the `allPacketsOut` output port. | Unit test |
| SVC-PASS_THROUGH_ROUTER-002 | `Svc::PassThroughRouter` shall return ownership of all buffers received on `dataIn` through `dataReturnOut` | Unit test |
