# Svc::Ccsds::SpacePacketFramer

The `Svc::Ccsds::SpacePacketFramer` is an implementation of the [FramerInterface](../../../Interfaces/docs/sdd.md) for the CCSDS [Space Packet Protocol](https://public.ccsds.org/Pubs/133x0b2e1.pdf).

It receives user data on its input port and constructs a CCSDS Space Packet. Please refer to the CCSDS [Space Packet Protocol specification (CCSDS 133.0-B-2)](https://public.ccsds.org/Pubs/133x0b2e1.pdf) for details on the packet format.

The `Svc::Ccsds::SpacePacketFramer` is typically used upstream of a component that adds transfer frame headers, such as the `Svc::Ccsds::TmFramer`. It encapsulates user data into a Space Packet, adding the necessary header fields.

## Configuration
The `Svc::Ccsds::SpacePacketFramer` requires an Application Process Identifier (APID) for the Space Packets it generates. This APID is typically provided during instantiation or configuration. It also uses a sequence count, which is managed per APID via the `getApidSeqCount` port.

## CCSDS Header Fields

For each Space Packet generated, the `Svc::Ccsds::SpacePacketFramer` will populate the CCSDS Space Packet Primary Header fields as follows:

| Field | Value | Notes |
|---|---|---|
| Version Number | 000 | As per protocol 4.1.3.2 |
| Packet Type | 0 (Telemetry) | SpacePacketFramer emits reporting packets only (no commanding), as per 4.1.3.3.2 |
| Secondary Header Flag | 0 | F Prime does not use secondary headers formally |
| Application Process Identifier (APID) | Uses value passed in the `context` argument | Project APIDs are defined in `config/ComCfg.fpp` |
| Sequence Flags | `0b11` (Unsegmented) | Unsegmented user data, F´ data fits in a single packet |
| Packet Sequence Count | Incremented for each packet, unique count per APID | Managed externally by a [`Svc::Ccsds::ApidManager`](../../ApidManager/docs/sdd.md) |
| Packet Data Length | Set to the length of the passed in data | Calculated based on the length of the data received on `dataIn` |

## Port Descriptions

| Kind | Name | Port Type | Description |
|---|---|---|---|
| Input (sync) | dataIn | Svc.ComDataWithContext | Port to receive user data to be framed into a Space Packet |
| Output | dataOut | Svc.ComDataWithContext | Port to output the constructed Space Packet |
| Output | bufferAllocate | Fw.BufferGet | Port to allocate buffers for the outgoing Space Packet |
| Output | bufferDeallocate | Fw.BufferSend | Port to deallocate buffers after the Space Packet is sent |
| Output | getApidSeqCount | Ccsds.ApidSequenceCount | Port to retrieve the current sequence count for a given APID |

## Requirements

| Name | Description | Validation |
|---|---|---|
| SPF-001 | The SpacePacketFramer shall implement the `Svc.FramerInterface`. | Inspection, Unit Test |
| SPF-002 | The SpacePacketFramer shall construct CCSDS Space Packets compliant with the CCSDS 133.0-B-2 standard. | Unit Test, Inspection |
| SPF-003 | The SpacePacketFramer shall accept user data to be framed via its `dataIn` port. | Unit Test |
| SPF-004 | The SpacePacketFramer shall output the constructed Space Packet via its `dataOut` port. | Unit Test |
| SPF-005 | The SpacePacketFramer shall use the `bufferAllocate` port to request memory buffers for outgoing Space Packets. | Unit Test |
| SPF-006 | The SpacePacketFramer shall use the `bufferDeallocate` port to return ownership of buffers after transmission. | Unit Test |
| SPF-007 | The SpacePacketFramer shall utilize the `getApidSeqCount` port to obtain the correct sequence count for the configured APID before framing a packet. | Unit Test |
| SPF-008 | The SpacePacketFramer shall correctly populate all mandatory fields of the Space Packet Primary Header, including Version Number, Packet Type, Secondary Header Flag, APID, Sequence Flags, Packet Sequence Count, and Packet Data Length. | Unit Test |
| SPF-009 | The SpacePacketFramer shall be configurable with an Application Process Identifier (APID) to be used in the Space Packet Header. | Inspection, Unit Test |
| SPF-010 | The SpacePacketFramer shall accurately calculate and set the Packet Data Length field in the Space Packet header based on the length of the user data. | Unit Test |
