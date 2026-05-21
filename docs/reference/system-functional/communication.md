
# Communication Stack Functionality

## References

- [F Prime Protocol SDD](https://github.com/nasa/fprime/blob/devel/Svc/FprimeProtocol/docs/sdd.md)
- [F Prime Framer SDD](https://github.com/nasa/fprime/blob/devel/Svc/FprimeFramer/docs/sdd.md)
- [F Prime Deframer SDD](https://github.com/nasa/fprime/blob/devel/Svc/FprimeDeframer/docs/sdd.md)
- [F Prime Router SDD](https://github.com/nasa/fprime/blob/devel/Svc/FprimeRouter/docs/sdd.md)
- [F Prime FrameAccumulator SDD](https://github.com/nasa/fprime/blob/devel/Svc/FrameAccumulator/docs/sdd.md)
- [F Prime ComQueue SDD](https://github.com/nasa/fprime/blob/devel/Svc/ComQueue/docs/sdd.md)
- [F Prime ComStub SDD](https://github.com/nasa/fprime/blob/devel/Svc/ComStub/docs/sdd.md)
- [F Prime ComRetry SDD](https://github.com/nasa/fprime/blob/devel/Svc/ComRetry/docs/sdd.md)
- [F Prime ComSplitter SDD](https://github.com/nasa/fprime/blob/devel/Svc/ComSplitter/docs/sdd.md)
- [F Prime ComAggregator SDD](https://github.com/nasa/fprime/blob/devel/Svc/ComAggregator/docs/sdd.md)
- [F Prime ComLogger SDD](https://github.com/nasa/fprime/blob/devel/Svc/ComLogger/docs/sdd.md)
- [F Prime CmdSplitter SDD](https://github.com/nasa/fprime/blob/devel/Svc/CmdSplitter/docs/sdd.md)
- [F Prime GenericHub SDD](https://github.com/nasa/fprime/blob/devel/Svc/GenericHub/docs/sdd.md)
- [F Prime PassThroughRouter SDD](https://github.com/nasa/fprime/blob/devel/Svc/PassThroughRouter/docs/sdd.md)
- [Communication Adapter Interface](https://github.com/nasa/fprime/blob/devel/docs/reference/communication-adapter-interface.md)

## Overview

The communication stack provides the data path between the flight software and external systems (ground station, other processors, or other deployments). It handles outgoing data (downlink) by queuing, framing, and transmitting packets, and incoming data (uplink) by accumulating byte streams, extracting frames, and routing packets to their destination components. The stack is modular and composed from interchangeable components that implement defined interfaces, allowing different protocol layers to be swapped in depending on mission requirements.

### Downlink Path

Outgoing data flows through the following stages:

1. **Queuing** — The Communication Queue receives data from multiple sources (telemetry, events, file packets) and prioritizes them for transmission. The queue supports configurable depth and priority levels, sending the highest-priority data first. Flow control is managed through a ready signal from downstream components — the queue only sends the next item when the downstream path signals readiness.

2. **Framing** — The framer wraps each outgoing packet in a protocol-specific frame. The default F Prime protocol adds a start word, packet size, and a CRC hash for integrity checking. The framing interface is pluggable, allowing alternative protocols (such as CCSDS) to be substituted.

3. **Transmission** — The framed data is sent to a communication adapter (typically a byte stream driver) for physical transmission. The adapter reports success or failure back through the communication status protocol.

### Uplink Path

Incoming data flows through the following stages:

1. **Accumulation** — The Frame Accumulator receives a stream of byte buffers (typically from a byte stream driver) and extracts complete frames. It handles the case where frames span multiple incoming buffers or where a single buffer contains multiple frames.

2. **Deframing** — The deframer validates the frame (checking CRC and structure) and extracts the payload. For the F Prime protocol, this involves verifying the start word, checking the size field, and validating the trailing CRC.

3. **Routing** — The router examines the extracted packet and forwards it to the appropriate destination. For the F Prime protocol, command packets are sent to the command dispatcher and file packets are sent to file uplink. The routing interface supports both protocol-aware routing (F Prime Router) and simple pass-through routing for single-destination configurations.

### F Prime Protocol

The default F Prime protocol is a minimal framing protocol consisting of four fields:

1. Start word — identifies the beginning of a frame
2. Packet size — the size of the enclosed payload
3. Payload data — the enclosed packet
4. CRC hash — integrity check covering the entire frame

This protocol is designed for simplicity and is primarily intended for development and testing with the F Prime GDS. For missions requiring more robust protocols, the CCSDS protocol components can be used instead.

### Retry Mechanism

The retry component sits in the downlink path before the communication adapter and resends failed transmissions up to a configurable maximum number of retries. After all retries are exhausted, it propagates the failure upstream. This provides resilience against transient communication failures.

### Communication Logging

The communication logger records all outgoing data to files on the file system, providing a record of transmitted data for debugging and analysis. Log files are rotated based on a configurable size or byte limit.

### Splitting and Aggregation

- **Command Splitter** — Duplicates incoming command buffers to multiple outputs, enabling redundant command processing paths.
- **Communication Splitter** — Distributes outgoing communication buffers to multiple output ports.
- **Communication Aggregator** — Merges data from multiple input sources into a single output stream.

### Hub Pattern (Cross-Deployment Communication)

The Generic Hub bridges port calls across address space boundaries between F Prime deployments. It serializes outgoing port calls for transmission to a remote deployment and deserializes incoming calls from the remote side. This enables distributed F Prime systems where components in different processes or on different processors can communicate transparently.

### Off Nominal

- Frame validation failures (bad CRC, malformed frames) cause the frame to be dropped with a warning event.
- Queue overflow causes data to be dropped according to the queue's configured overflow behavior.
- Communication adapter failures are reported upstream, and the retry mechanism (if present) attempts retransmission.
- If the communication path is down, the communication queue will fill and begin dropping lower-priority data.
