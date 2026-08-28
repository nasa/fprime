# Add Custom Uplink and Downlink Data Types

Out of the box, F´ uplinks commands and file packets, and downlinks telemetry, events, file packets, and data products. Each of these data types is identified by an APID (Application Process Identifier) defined in the `ComCfg::Apid` enumeration. Some projects need to transfer additional, project-specific data through the communication stack — for example raw science data on downlink, or a custom payload data stream on uplink.

This guide shows how to add a project-specific data type identified by a custom APID, covering:

1. Defining the custom APID in the project configuration
2. Downlinking custom data through `Svc.ComQueue`
3. Routing custom uplink data with `Svc.FprimeRouter`
4. Handling the custom data type in the F´ GDS

This guide assumes the standard CCSDS communication stack (`Svc.ComCcsds` subtopology). The same concepts apply to the F´ Protocol stack (`Svc.ComFprime`), where the APID acts as the packet descriptor. For background, see the [Communication Stack](../../reference/system-functional/communication.md) and [CCSDS Protocol](../../reference/system-functional/ccsds-protocol.md) references.

## Packet Format Overview

Inside the communication stack, every F´ packet has the same simple structure: a packet descriptor (the APID) followed by user data.

```
+---------------------------+---------------------------------------------+
| APID / Packet Descriptor  | User Data                                   |
| (FwPacketDescriptorType,  | (format defined by the data type)           |
|  2 bytes by default)      |                                             |
+---------------------------+---------------------------------------------+
```

The descriptor tells the stack (and the ground system) how to interpret the user data. For the standard data types the user data is defined by the framework, for example:

| APID | User Data |
|---|---|
| `FW_PACKET_COMMAND` | `[opcode][serialized command arguments]` |
| `FW_PACKET_TELEM` | `[channel ID][time tag][serialized channel value]` |
| `FW_PACKET_LOG` | `[event ID][time tag][serialized event arguments]` |
| `FW_PACKET_FILE` | `[file packet: START/DATA/END/CANCEL]` |
| `MY_PROJECT_DATA` (custom) | `[project-defined fields]` |

For a custom APID, the user data format is entirely up to the project — the framework only reads the leading descriptor and passes the rest through opaquely.

When the CCSDS stack is used, each such packet is carried as the payload of a CCSDS Space Packet ([CCSDS 133.0-B-2](https://ccsds.org/Pubs/133x0b2e2.pdf)), whose 6-byte primary header carries the APID and a per-APID sequence count:

```
+--------------------------------------+---------------------------------------------+
| Space Packet Header (6 bytes)        | Packet Data Field                          |
| (version, type, APID, sequence,      | F´ packet: APID / descriptor + user data   |
|  length, and other CCSDS fields)     |                                             |
+--------------------------------------+---------------------------------------------+
```

The Space Packet header APID is filled from the same `ComCfg::Apid` value, which is why a custom data type only needs one new enumeration entry to be identified consistently at every layer.

## 1. Define the Custom APID

APIDs are defined in the `ComCfg::Apid` enumeration in `config/ComCfg.fpp`. Projects override this file through the standard [configuration override mechanism](../../user-manual/build-system/settings.md) (`config_directory` in `settings.ini`).

Copy `ComCfg.fpp` into your project configuration directory and add your APID to the enumeration, keeping all the reserved `FW_*` entries:

```fpp
@ APIDs are 11 bits in the Space Packet protocol, so we use U16. Max value 7FF
dictionary enum Apid : FwPacketDescriptorType {
    # APIDs prefixed with FW are reserved for F Prime and need to be present
    FW_PACKET_COMMAND        = 0x0000  @< Command packet type - incoming
    # ... all other FW_* entries ...

    MY_PROJECT_DATA          = 0x0100  @< Project-specific data - incoming and/or outgoing

    SPP_IDLE_PACKET          = 0x07FF
    INVALID_UNINITIALIZED    = 0x0800
} default INVALID_UNINITIALIZED
```

Notes:

- APIDs are 11 bits in the CCSDS Space Packet protocol; values must be below `0x7FF`.
- The APID **must** be a member of the enumeration: on uplink, `Svc.Ccsds.SpacePacketDeframer` maps any value that is not a valid `ComCfg::Apid` member to `FW_PACKET_UNKNOWN`.
- The reserved `FW_*` entries must keep values matching `Fw::ComPacketType` (this is enforced with `static_assert` in `Svc.Ccsds.ApidManager`).
- The `Svc.Ccsds.ApidManager` sequence-count table sizes itself to the number of entries in the `Apid` enumeration, so new APIDs are tracked automatically.

## 2. Downlink: Sending Custom Data

### How the framework identifies outgoing data

`Svc.ComQueue` determines the APID of outgoing data by deserializing the first `FwPacketDescriptorType` bytes of each queued buffer and storing the value in the `ComCfg::FrameContext` passed down the framing stack. The `Svc.Ccsds.SpacePacketFramer` then uses `context.apid` to fill the Space Packet header.

Therefore, a custom downlink producer must serialize its APID at the very start of the data buffer:

```cpp
Fw::Buffer buffer = /* allocated from a buffer manager */;
auto serializer = buffer.getSerializer();
Fw::SerializeStatus status = serializer.serializeFrom(
    static_cast<FwPacketDescriptorType>(ComCfg::Apid::MY_PROJECT_DATA));
// ... serialize payload data ...
this->dataOut_out(0, buffer);
```

### Add a queue input port

`Svc.ComQueue` has a configurable number of `Fw::Com` and `Fw::Buffer` input ports, set by the `ComQueueComPorts` and `ComQueueBufferPorts` constants in `config/AcConstants.fpp`. Override this file in your project configuration and increment the relevant constant, e.g.:

```fpp
@ Used for number of Fw::Buffer type ports supported by Svc::ComQueue
constant ComQueueBufferPorts = 2
```

### Wire the producer into the topology

Give your producer component an `Fw.BufferSend` output port (and an `Fw.BufferSend` input port to receive buffer ownership back), then connect it to the free `ComQueue` port index. When using the `Svc.ComCcsds` subtopology, the queue's port arrays are exposed as topology ports:

```fpp
connections MyProjectDownlink {
    myProducer.dataOut -> ComCcsds.Subtopology.bufferQueueIn[1]
    ComCcsds.Subtopology.bufferReturnOut[1] -> myProducer.dataReturnIn
}
```

### Configure queue depth and priority

`Svc.ComQueue` requires a depth and priority for every queue entry in its `QueueConfigurationTable`. The subtopology configures the standard entries in its `configComponents` phase (see `Svc/Subtopologies/ComCcsds/ComCcsds.fpp`); the entry for the new buffer port must be added. Buffer queue entries are indexed after the `Fw::Com` entries:

```cpp
configurationTable.entries[Ports_ComPacketQueue::NUM_CONSTANTS + 1].depth = 10;
configurationTable.entries[Ports_ComPacketQueue::NUM_CONSTANTS + 1].priority = 3;
```

Depending on how much of the subtopology configuration your project overrides, this is done either by overriding the subtopology configuration or by copying the relevant phase code into your own topology (see the [Subtopologies Guide](../../user-manual/design-patterns/subtopologies.md)).

### Buffer ownership

Buffers sent to `Svc.ComQueue` are returned on the matching `bufferReturnOut` port after the data has been framed and sent. The producer owns deallocation: return the buffer to the buffer manager it was allocated from when it comes back on `dataReturnIn`.

> [!IMPORTANT]
> Every buffer must be returned through the matching ownership-return port after processing. Failing to return a buffer results in a memory leak and can eventually exhaust the communications buffer pool.

## 3. Uplink: Receiving Custom Data

### How the framework routes incoming data

On uplink, the `Svc.Ccsds.SpacePacketDeframer` extracts the APID from the Space Packet header and stores it in the `FrameContext`. The [`Svc.FprimeRouter`](../../../Svc/FprimeRouter/docs/sdd.md) routes command and file packets to their standard destinations, and forwards **all other packet types** — including project-specific APIDs — on its `unknownDataOut` port.

### Implement a project router or handler

Create a component with:

- a `Svc.ComDataWithContext` input port to receive the forwarded data, and
- an `Fw.BufferSend` output port to return buffer ownership.

In the handler, dispatch on the APID carried in the context:

```cpp
void MyProjectRouter ::dataIn_handler(FwIndexType portNum,
                                      Fw::Buffer& data,
                                      const ComCfg::FrameContext& context) {
    if (context.get_apid() == ComCfg::Apid::MY_PROJECT_DATA) {
        // ... process the payload in `data` ...
    }
    // Return ownership of the buffer to the router when done
    this->bufferReturnOut_out(0, data);
}
```

> [!IMPORTANT]
> Buffers received on `unknownDataOut` are passed without copying and **must** be returned to `Svc.FprimeRouter` through its `fileBufferReturnIn` port when processing is complete. Failing to do so results in a memory leak and can eventually exhaust the uplink buffer pool.

### Wire it into the topology

```fpp
connections MyProjectUplink {
    ComCcsds.fprimeRouter.unknownDataOut -> myProjectRouter.dataIn
    myProjectRouter.bufferReturnOut -> ComCcsds.fprimeRouter.fileBufferReturnIn
}
```

If your project needs more elaborate routing (multiple custom APIDs to multiple destinations), the project router can fan out to further components; it remains the single owner responsible for returning each buffer.

## 4. GDS Integration

The F´ GDS natively produces and consumes only the standard F´ data types. Project-specific APIDs require GDS extensions through the [plugin system](../operate/develop-gds-plugins.md):

> [!NOTE]
> Custom APID and data-type support is outside the primary scope of the F´ GDS. The available support is minimal and projects should expect to provide their own GDS plugin or application code.

- **Downlink**: implement a [Data Handler plugin](../../reference/gds-plugins/data-handler.md) registered for your custom descriptor to consume the data on the ground (log it, forward it to another service, etc.).
- **Uplink**: sending custom-APID data from the ground is project-specific; a common approach is a custom GDS application or plugin that builds the packet (APID + payload) and submits it through the GDS uplink path.

If you are integrating with a non-F´ ground system, refer to that GDS's documentation for instructions on defining and handling custom packet types and APIDs.

## References

- [CCSDS Protocol Functionality](../../reference/system-functional/ccsds-protocol.md)
- [Communication Stack Functionality](../../reference/system-functional/communication.md)
- [Svc.FprimeRouter SDD](../../../Svc/FprimeRouter/docs/sdd.md)
- [Svc.ComQueue SDD](../../../Svc/ComQueue/docs/sdd.md)
- [Svc.Ccsds.ApidManager SDD](../../../Svc/Ccsds/ApidManager/docs/sdd.md)
- [Subtopologies Guide](../../user-manual/design-patterns/subtopologies.md)
- [GDS Plugin Development](../operate/develop-gds-plugins.md)
