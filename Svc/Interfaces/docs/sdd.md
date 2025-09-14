# Svc FPP Interfaces

The Svc interfaces are a set of `.fppi` files that define FPP interfaces for components to implement. An FPP interface is an FPP file that defines a set of ports. A component that implements an FPP interface must implement handlers for the input ports and has access to the output ports of the interface.

## Svc/ComInterface

The `Svc/ComInterface` is an interface for implementing the [Communications Adapter Interface](../../../docs/reference/communication-adapter-interface.md). [`Svc::ComStub`](../../ComStub/docs/sdd.md) implements this interface and uses a ByteStream driver to send and receive data on a TCP/UDP/UART link, and is often used in development and early testing.

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
