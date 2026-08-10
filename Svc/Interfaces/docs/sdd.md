# Svc FPP Interfaces

The Svc interfaces are a set of `.fppi` files that define FPP interfaces for components to implement. An FPP interface is an FPP file that defines a set of ports. A component that implements an FPP interface must implement handlers for the input ports and has access to the output ports of the interface.

## Svc/ComInterface

The `Svc/ComInterface` is an interface for implementing the [Communications Adapter Interface](../../../docs/reference/communication-adapter-interface.md). Components implementing this interface must follow the [Communication Adapter Protocol](../../../docs/reference/communication-adapter-interface.md#communication-adapter-protocol), which defines when `Fw::Success::SUCCESS` and `Fw::Success::FAILURE` may be emitted via `comStatusOut`. [`Svc::ComStub`](../../ComStub/docs/sdd.md) implements this interface and uses a ByteStream driver to send and receive data on a TCP/UDP/UART link, and is often used in development and early testing.

## Svc/DeframerInterface

The `Svc/DeframerInterface` is an interface for implementing a Deframer component. This interface allows a component to be dropped in the common F´ Uplink stack and implement deframing for a specific communications protocol. The [`Svc::FprimeDeframer`](../../FprimeDeframer/docs/sdd.md) component implements this interface for the [F´ communications protocol](../../FprimeProtocol/docs/sdd.md).

## Svc/FramerInterface

The `Svc/FramerInterface` is an interface for implementing the Framer component. This interface allows a component to be dropped in the common F´ Downlink stack and implement framing for a specific communications protocol. The [`Svc::FprimeFramer`](../../FprimeFramer/docs/sdd.md) component implements this interface for the [F´ communications protocol](../../FprimeProtocol/docs/sdd.md).

## Svc/RouterInterface

The `Svc/RouterInterface` is an interface for implementing a Router component. This interface allows a component to be dropped in the common F´ Uplink stack and implement routing for a project. The [`Svc::FprimeRouter`](../../FprimeRouter/docs/sdd.md) component implements this interface routing the common F´ packets, as well as passing unknown packets down to another component for further processing.

## Svc/FrameAccumulatorInterface

The `Svc/FrameAccumulatorInterface` is an interface for implementing a Frame Accumulator component. This allows a component to be dropped in the common F´ Uplink stack and implement frame accumulation, using any desirable algorithm. The [`Svc::FrameAccumulator`](../../FrameAccumulator/docs/sdd.md) component implements this interface by storing the data in a circular buffer and using a `Svc::FrameDetector` to detect frames in the buffer.

## Svc/TimeInterface

The `Svc/TimeInterface` is an interface for implementing a Time provider. A time provider must provide a way for other components to get the current time. The [`Svc::PosixTime`](../../PosixTime/docs/sdd.md) component implements this interface, using common Posix functions to retrieve the current time.

<!-- fpp-dictionary-begin -->
## Interface Dictionary

The following tables are derived from the interface FPP models in this directory.

### Port Descriptions

| Name | Kind | Port Type | Description |
|---|---|---|---|
| `allocate` | `output` | `Fw.BufferGet` | Allocation request to buffer manager |
| `deallocate` | `output` | `Fw.BufferSend` | Deallocation request to buffer manager |
| `dataIn` | `sync input` | `Svc.ComDataWithContext` | Data to be sent on the wire (coming in to the component) |
| `dataOut` | `output` | `Svc.ComDataWithContext` | Data received from the wire (going out of the component) |
| `comStatusOut` | `output` | `Fw.SuccessCondition` | Status of the last transmission |
| `dataReturnOut` | `output` | `Svc.ComDataWithContext` | Port returning ownership of data that came in on dataIn |
| `dataReturnIn` | `sync input` | `Svc.ComDataWithContext` | Port receiving back ownership of buffer sent out on dataOut |
| `dataIn` | `guarded input` | `Svc.ComDataWithContext` | Port to receive framed data, with optional context |
| `dataOut` | `output` | `Svc.ComDataWithContext` | Port to output deframed data, with optional context |
| `dataReturnOut` | `output` | `Svc.ComDataWithContext` | Port for returning ownership of received buffers to deframe |
| `dataReturnIn` | `sync input` | `Svc.ComDataWithContext` | Port receiving back ownership of sent buffers |
| `dataIn` | `guarded input` | `Svc.ComDataWithContext` | Receive raw bytes from a ComInterface (e.g. ComStub) |
| `dataOut` | `output` | `Svc.ComDataWithContext` | Port for sending an extracted frame out |
| `dataReturnOut` | `output` | `Svc.ComDataWithContext` | Port for returning ownership of buffers received on dataIn |
| `dataReturnIn` | `sync input` | `Svc.ComDataWithContext` | Port receiving back ownership of buffers sent on frameOut |
| `dataIn` | `sync input` | `Svc.ComDataWithContext` | Port to receive data to frame, in a Fw::Buffer with optional context |
| `dataOut` | `output` | `Svc.ComDataWithContext` | Port to output framed data with optional context |
| `dataReturnOut` | `output` | `Svc.ComDataWithContext` | Port for returning ownership of the incoming Fw::Buffer to its sender once framing is handled |
| `dataReturnIn` | `sync input` | `Svc.ComDataWithContext` | Buffer coming from a deallocate call in a ComDriver component |
| `comStatusIn` | `sync input` | `Fw.SuccessCondition` | Port receiving the general status from the downstream component indicating it is ready or not-ready for more input |
| `comStatusOut` | `output` | `Fw.SuccessCondition` | Port receiving indicating the status of framer for receiving more data |
| `dataIn` | `guarded input` | `Svc.ComDataWithContext` | Receiving data (Fw::Buffer) to be routed with optional context to help with routing |
| `dataReturnOut` | `output` | `Svc.ComDataWithContext` | Port for returning ownership of data (includes Fw.Buffer) received on dataIn |
| `fileOut` | `output` | `Fw.BufferSend` | Port for sending file packets as Fw::Buffer (ownership passed to receiver) |
| `fileBufferReturnIn` | `guarded input` | `Fw.BufferSend` | Port for receiving back ownership of buffers sent on fileOut or any other output port that passes buffer ownership to the receiver |
| `commandOut` | `output` | `Fw.Com` | Port for sending command packets as Fw::ComBuffers |
| `cmdResponseIn` | `sync input` | `Fw.CmdResponse` | Port for receiving command responses from a command dispatcher (can be a no-op) |
| `timeGetPort` | `sync input` | `Fw.Time` | Port to retrieve time |

<!-- fpp-dictionary-end -->
