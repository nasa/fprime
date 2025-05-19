# Svc::CCSDS::ApidMapper

## 1. Overview

The `ApidMapper` component is a passive F´ component that maps the descriptor type (ComPacketType/APID) found in the first two bytes of a data buffer to a CCSDS Space Packet APID. It also tracks and manages a sequence count for each APID, as required by the CCSDS protocol. This enables downstream components to generate correct CCSDS Space Packets with the appropriate APID and sequence count.

## 2. Requirements

- Extract the APID (descriptor type) from the first two bytes of each incoming buffer.
- Set the APID in the outgoing `FrameContext`.
- Track a 14-bit sequence count for each APID, incrementing and wrapping as required by the CCSDS standard.
- Support a small, fixed set of non-contiguous APIDs.
- Operate without dynamic memory allocation or standard library containers.

## 3. Interfaces

### Ports

| Port Name      | Direction | Description                                                      |
|----------------|-----------|------------------------------------------------------------------|
| dataIn         | input     | Receives data buffers and context from upstream                  |
| dataOut        | output    | Forwards buffer with updated context (APID and sequence count)   |
| dataReturnIn   | input     | Receives returned buffers from downstream                        |
| dataReturnOut  | output    | Returns buffers to upstream                                      |
| comStatusIn    | input     | Receives status from downstream                                  |
| comStatusOut   | output    | Forwards status upstream                                         |
