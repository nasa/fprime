# Svc::ComBufferToComAdapter

The `ComBufferToComAdapter` provides backwards compatibility between the
`Fw.ComBufferSend` interface and the legacy `Fw.Com` interface.

The newer `Fw.ComBufferSend` interface carries the packet descriptor (APID) as
an explicit `packetType` argument alongside the payload buffer. In the legacy
format, that descriptor is instead serialized as a `FwPacketDescriptorType` at
the front of the `Fw::ComBuffer`.

This component bridges a new-style producer (which emits `Fw.ComBufferSend`) to
a legacy consumer (which expects `Fw.Com`).

## Behavior

On each `comBufferSendIn` invocation the component:

1. Serializes the `packetType` (APID) as a `FwPacketDescriptorType` at the front
   of a new `Fw::ComBuffer`.
2. Appends the original payload bytes after the descriptor.
3. Forwards the resulting buffer on `comOut`, preserving the `context` value.

See `Svc::ComToComBufferAdapter` for the reverse direction.

## Ports

| Kind | Name | Type | Description |
|------|------|------|-------------|
| `sync input` | `comBufferSendIn` | `Fw.ComBufferSend` | Payload with the descriptor as the `packetType` argument |
| `output` | `comOut` | `Fw.Com` | Legacy buffer with a `FwPacketDescriptorType` prepended |

## Requirements

| Name | Description | Validation |
|------|-------------|------------|
| COM-ADAPT-101 | The component shall serialize the `packetType` as a `FwPacketDescriptorType` at the front of the output buffer. | Unit test |
| COM-ADAPT-102 | The component shall append the original payload bytes after the prepended descriptor. | Unit test |
| COM-ADAPT-103 | The component shall preserve the `context` value across the conversion. | Unit test |
