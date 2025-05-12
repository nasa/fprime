# Svc::FprimeFramer

The `Svc::FprimeFramer` is an implementation of the [FramerInterface](../../Interfaces/docs/sdd.md) for the F Prime protocol. 

It receives data (an F´ packet) on input and produces an [F´ frame](../../FprimeProtocol/docs/sdd.md) on its output port as a result. Please refer to the [F Prime frame specification](../../FprimeProtocol/docs/sdd.md) for details on the frame format.

It is designed to receive packets from a [`Svc::ComQueue`](../../ComQueue/docs/sdd.md) and passes frames to a [Communications Adapter](../../Interfaces/docs/sdd.md), such as a Radio manager component or [`Svc::ComStub`](../../ComStub/docs/sdd.md), for transmission on the wire.

## Usage Examples

The `Svc::FprimeFramer` component is used in the uplink stack of many reference F´ application such as [the tutorials source code](https://github.com/fprime-community#tutorials).

## Internals

The `Svc::FprimeFramer` receives data packets of type `Svc.ComDataWithContext`. This type contains both a `Fw::Buffer` containing the packet data, and a `context: FrameContext` that contains contextual information about the data packet (such as an APID). In the default configuration (using Svc::ComQueue), the `context` is used to determine whether a packet is coming from the ComQueue's Fw::Buffer queue (as opposed to ComPacket queue). If it is, the original data packet `Fw::Buffer` is returned back to its original sender.

On receiving a data packet, the `Svc::FprimeFramer` performs the following actions:

1. Allocates a new _`outBuffer`_ (of type `Fw::Buffer`) to hold the F´ frame, of size _`size(dataPacket) + size(FprimeHeader) + size(FprimeTrailer)`_
2. Serializes the F´ start word (`0xDEADBEEF`) and length token (`size(dataPacket)`) into _`outBuffer`_
3. Serializes the F´ packet data into _`outBuffer`_
4. Computes and serializes a CRC32 checksum into _`outBuffer`_
5. Emits the _`outBuffer`_ on the `dataOut` output port. Ownership of _`outBuffer`_ is handed to the receiver
5. Transfer ownership of input _`dataPacket`_ to the `dataReturnOut` port. This usually should be connected to the same component that sent the original packet to `dataIn`.

## Port Descriptions

| Kind            | Name            | Port Type                | Usage                                                                    |
|-----------------|-----------------|--------------------------|--------------------------------------------------------------------------|
| `guarded input` | `dataIn`        | `Svc.ComDataWithContext` | Port to receive data to frame, in a Fw::Buffer with optional context     |
| `output`        | `dataOut`       | `Svc.ComDataWithContext` | Port to output framed data, with optional context, for follow-up framing |
| `sync input`    | `dataReturnIn`  | `Svc.ComDataWithContext` | Port to receive back ownership of buffer sent out of `dataOut`           |
| `output`        | `dataReturnOut` | `Svc.ComDataWithContext` | Port to return ownership of buffer received on `dataIn`                  |
| `sync input`    | `comStatusIn`   | `Fw.SuccessCondition`    | Port receiving the general status from the downstream component          |
| `output`        | `comStatusOut`  | `Fw.SuccessCondition`    | Port receiving indicating the status of framer for receiving more data   |

## Requirements

| Name | Description | Validation |
|---|---|---|
| SVC-FPRIME_FRAMER-001 | `Svc::FprimeFramer` shall accept data buffers (packets) stored in `Fw::Buffer` through its `dataIn` input port | Unit Test |
| SVC-FPRIME_FRAMER-002 | `Svc::FprimeFramer` shall emit one F Prime frame on its `framedOut` output port for each packet received on `dataIn` input port | Unit Test |
| SVC-FPRIME_FRAMER-003 | `Svc::FprimeFramer` shall emit F Prime frames that conforms to the [F´ frame specification](../../FprimeProtocol/docs/sdd.md) | Unit Test |
| SVC-FPRIME_FRAMER-004 | `Svc::FprimeFramer` shall pass through all `Fw.SuccessCondition` received on `comStatusIn` to `comStatusOut` | Unit Test |

