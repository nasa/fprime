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

The fill target defaults to `ComCfg.SdlsFillTargetSize`, so no topology setup call is required. That constant is the plaintext size which exactly fills the transfer frame data field once framing and security overhead are added, and it is derived from the frame geometry rather than written out:

```
SdlsFillTargetSize = TmFrameFixedSize - 6 (TM header) - 2 (SPI) - 2 (FECF)   # 1014 by default
AggregationSize    = SdlsFillTargetSize - 7                                   # 1007 by default
```

A project selecting a real encryptor subtracts that component's overhead as well, by overriding `ComCfg` through CMake `CONFIGURATION_OVERRIDES`. For AES-256-GCM that is 28 bytes more (12-byte IV plus 16-byte MAC), giving `1024 - 6 - 2 - 2 - 12 - 16 = 986`, with `AggregationSize` 979.

**Both constants must move together.** `AggregationSize` caps the upstream aggregation buffer, and the subtraction of 7 is what guarantees every aggregate either fills the target exactly or leaves room for a well-formed idle Space Packet. Defining it as `SdlsFillTargetSize - 7`, as the default does, preserves the relation automatically; writing an independent literal invites the two to drift.

Getting it wrong has these consequences, which is why the component `static_assert`s both bounds rather than leaving them to run time:

| Misconfiguration | Effect |
|---|---|
| `AggregationSize` > target | Every aggregate above the target is dropped with `InputTooLarge`; with the AES target of 986 and the pre-existing default of 1009, that is most full frames |
| target − 7 < `AggregationSize` ≤ target | Aggregates in the last 6 bytes of the range are dropped with `GapTooSmall` — a partial, size-dependent outage that a log skim can easily miss |
| target > frame data field | The padded buffer cannot fit the frame at all |

The first two are caught by `static_assert(SdlsFillTargetSize >= AggregationSize + 7)` and the third by `static_assert(SdlsFillTargetSize <= MAX_FILL_SIZE)`, both in the component header, so a project that overrides one constant and forgets the other fails to build.

`configure(FwSizeType targetSize)` remains available as an optional override, for a deployment pairing this component with an upstream aggregator sized differently from the compile-time configuration. It asserts only the intrinsic bounds (`targetSize` in `[1, MAX_FILL_SIZE]`); the relation to `ComCfg.AggregationSize` is deliberately not checked there, since that constant describes the default target rather than an overridden one. Call it during topology setup, before any buffer is padded.

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
| SVC-CCSDS-SPACE-PACKET-IDLE-FILLER-011 | The fill target shall default to `ComCfg.SdlsFillTargetSize`, so that the component operates correctly with no topology setup call. | Unit Test |
| SVC-CCSDS-SPACE-PACKET-IDLE-FILLER-012 | The component shall fail to compile if `ComCfg.SdlsFillTargetSize` exceeds the transfer frame data field, or if it is less than `ComCfg.AggregationSize` plus the minimum idle Space Packet size. | Inspection |

## Deployment Notes

Instantiated inside `ComCcsdsSdls.SdlsEncryption`, ahead of `sdlsFramer`, and so present in every topology built on it (`ComCcsdsSdls.FramingSubtopology`, `ComCcsdsSdls.Subtopology`). The non-SDLS `ComCcsds.Subtopology` has no encryption layer and does not instantiate it.

## See Also

- [`Svc/Ccsds/TmFramer`](../../TmFramer/docs/sdd.md)
- [`Svc/Ccsds/SpacePacketFramer`](../../SpacePacketFramer/docs/sdd.md)
- [`Svc/Ccsds/AesGcmEncryptor`](../../AesGcmEncryptor/docs/sdd.md)
