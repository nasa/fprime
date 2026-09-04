# Svc::Ccsds::SpacePacketIdleFiller

The `Svc::Ccsds::SpacePacketIdleFiller` component pads a buffer to a fixed size with a single CCSDS idle Space Packet, so a downstream framer finds the data field already full and adds no fill of its own. It implements the [FramerInterface](../../../Interfaces/docs/sdd.md), so it drops into a framing chain wherever a framer already sits. It is used on SDLS downlinks, ahead of SDLS encryption, so the fill lands inside the authenticated, encrypted region rather than behind it.

## Functionality

- Computes `gap = targetSize - inputSize` for each buffer received on `dataIn`.
- `gap` at least 7: copies the input, appends one idle Space Packet of `gap` bytes, and emits at `targetSize` on `dataOut`.
- `gap` of 0: copies the input and emits it unchanged at `targetSize`.
- `gap` of 1 to 6: emits the `GapTooSmall` event and drops the buffer (no conformant Space Packet fits in fewer than 7 bytes).
- `gap` negative: emits the `InputTooLarge` event and drops the buffer.
- Returns the incoming buffer on `dataReturnOut` as soon as it has been copied.
- A drop is followed by `Fw::Success::SUCCESS` on `comStatusOut`, releasing the com token so `Svc::ComAggregator` does not stall.
- Passes status received on `comStatusIn` through to `comStatusOut` unmodified.

The emitted idle packet matches what `Svc::Ccsds::TmFramer` produces: APID `0x7FF`, sequence flags `0b11` (unsegmented), an uncounted sequence number, packet data length `gap - 7`, and a `0x44` fill pattern.

## Port Descriptions

| Kind | Name | Port Type | Description |
|---|---|---|---|
| sync input | dataIn | Svc.ComDataWithContext | Receives the buffer to pad (from `Svc.Framer`). |
| output | dataOut | Svc.ComDataWithContext | Sends the padded buffer downstream, always exactly `targetSize` bytes. |
| output | dataReturnOut | Svc.ComDataWithContext | Returns ownership of the incoming buffer to its sender. |
| sync input | dataReturnIn | Svc.ComDataWithContext | Receives back ownership of the buffer sent on `dataOut`. |
| sync input | comStatusIn | Fw.SuccessCondition | Receives status from the downstream framing chain. |
| output | comStatusOut | Fw.SuccessCondition | Passes status upstream, unchanged. |

## Events

| Name | Severity | Description |
|---|---|---|
| InputTooLarge | WARNING_HI | A buffer larger than the configured fill target arrived. |
| GapTooSmall | WARNING_HI | A buffer arrived leaving too little room for a well-formed Space Packet. |

## Configuration

`configure(FwSizeType targetSize)` must be called during topology setup; a buffer arriving first asserts. `targetSize` is the plaintext size that exactly fills the transfer frame data field once framing and security overhead are added. For a 1024-byte TM frame carrying SDLS AES-256-GCM, `1024 - 6 (TM header) - 2 (FECF) - 2 (SPI) - 12 (IV) - 16 (MAC) = 986`.

The deployment must also cap its upstream aggregation buffer at `targetSize - 7`, so a buffer either fills the target exactly or leaves room for a well-formed idle packet

## Requirements

| Name | Description | Validation |
|---|---|---|
| SVC-CCSDS-SPACE-PACKET-IDLE-FILLER-001 | A buffer received on `dataIn` shall be emitted on `dataOut` at exactly the configured target size, with its bytes unchanged at the head. | Unit Test |
| SVC-CCSDS-SPACE-PACKET-IDLE-FILLER-002 | The padding shall be a single CCSDS idle Space Packet with APID `0x7FF`, sequence flags `0b11`, and a packet data length field of `gap - 7`. | Unit Test |
| SVC-CCSDS-SPACE-PACKET-IDLE-FILLER-003 | A buffer already at the target size shall be emitted with no packet appended. | Unit Test |
| SVC-CCSDS-SPACE-PACKET-IDLE-FILLER-004 | A buffer larger than the target size shall be dropped and shall raise `InputTooLarge`. | Unit Test |
| SVC-CCSDS-SPACE-PACKET-IDLE-FILLER-005 | A buffer leaving a gap too small for a well-formed Space Packet shall be dropped and shall raise `GapTooSmall`. | Unit Test |
| SVC-CCSDS-SPACE-PACKET-IDLE-FILLER-006 | The `ComCfg::FrameContext` received on `dataIn` shall be forwarded on `dataOut` unchanged. | Unit Test |
| SVC-CCSDS-SPACE-PACKET-IDLE-FILLER-007 | A buffer received on `dataIn` shall be returned on `dataReturnOut`, whether or not it was emitted. | Unit Test |
| SVC-CCSDS-SPACE-PACKET-IDLE-FILLER-008 | The emitted buffer returning on `dataReturnIn` shall free the component storage for the next buffer. | Unit Test |
| SVC-CCSDS-SPACE-PACKET-IDLE-FILLER-009 | A status received on `comStatusIn` shall be forwarded on `comStatusOut` unchanged. | Unit Test |
| SVC-CCSDS-SPACE-PACKET-IDLE-FILLER-010 | A dropped buffer shall be followed by `Fw::Success::SUCCESS` on `comStatusOut`. | Unit Test |

## Deployment Notes

Instantiated inside `ComCcsdsSdls.SdlsEncryption`, ahead of `sdlsFramer`, and so present in every topology built on it (`ComCcsdsSdls.FramingSubtopology`, `ComCcsdsSdls.Subtopology`). The non-SDLS `ComCcsds.Subtopology` has no encryption layer and does not instantiate it.

## See Also

- [`Svc/Ccsds/TmFramer`](../../TmFramer/docs/sdd.md)
- [`Svc/Ccsds/SpacePacketFramer`](../../SpacePacketFramer/docs/sdd.md)
- [`Svc/Ccsds/AesGcmEncryptor`](../../AesGcmEncryptor/docs/sdd.md)
