# Svc::ComToComBufferAdapter

The `ComToComBufferAdapter` provides backwards compatibility between the legacy
`Fw.Com` interface and the `Fw.ComBufferSend` interface.

In the legacy format, a `FwPacketDescriptorType` (the packet type / APID) is
serialized at the front of the `Fw::ComBuffer`. The newer `Fw.ComBufferSend`
interface instead carries the descriptor as an explicit `packetType` (APID)
argument alongside the payload buffer.

This component bridges a legacy producer (which emits `Fw.Com`) to a new-style
consumer (which expects `Fw.ComBufferSend`).

## Behavior

On each `comIn` invocation the component:

1. Reads the leading `FwPacketDescriptorType` from the front of the buffer.
2. Strips it, leaving only the payload bytes.
3. Forwards the payload on `comBufferSendOut`, passing the descriptor value as
   the `packetType` (APID) argument and preserving the `context` value.

See `Svc::ComBufferToComAdapter` for the reverse direction.

## Ports

| Kind | Name | Type | Description |
|------|------|------|-------------|
| `sync input` | `comIn` | `Fw.Com` | Legacy buffer with a `FwPacketDescriptorType` prepended |
| `output` | `comBufferSendOut` | `Fw.ComBufferSend` | Payload (descriptor stripped) with the descriptor as the `packetType` argument |

## Requirements

| Name | Description | Validation |
|------|-------------|------------|
| COM-ADAPT-001 | The component shall strip the leading `FwPacketDescriptorType` from the legacy buffer and forward the remaining payload. | Unit test |
| COM-ADAPT-002 | The component shall pass the stripped descriptor as the `packetType` argument on the output port. | Unit test |
| COM-ADAPT-003 | The component shall preserve the `context` value across the conversion. | Unit test |
