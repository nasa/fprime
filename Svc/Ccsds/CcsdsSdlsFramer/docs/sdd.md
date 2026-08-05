# Svc::Ccsds::CcsdsSdlsFramer

The `Svc::Ccsds::CcsdsSdlsFramer` component frames buffers into CCSDS SDLS (Space Data Link Security) frames. It is both a framer (implementing the `Svc.Framer` interface within a framing pipeline) and an encryption client (implementing `Svc.Ccsds.CcsdsSdlsEncryptClient` toward an encryption helper such as `Svc.Ccsds.SdlsSaRouter` or an encryptor component). It is the downlink mirror of [`Svc::Ccsds::CcsdsSdlsDeframer`](../../CcsdsSdlsDeframer/docs/sdd.md).

## Introduction

Framing an SDLS frame proceeds as follows:

1. Determine the security association (SA) index: from the frame context when set, otherwise from the `SA_INDEX` parameter.
2. Record the SA index in the frame context and pass the data, SA index, and context to the encryption helper.
3. Check the status passed forward with the encrypted data; on failure raise an event and return the buffer.
4. Upon receiving successfully encrypted data back, allocate a frame buffer and prepend the 16-bit SA index.
5. Pass the SDLS frame downstream for further framing/transmission.

Buffer ownership follows the standard F Prime data-with-context return pattern: encrypted buffers are returned to the encryption helper via `encryptReturnOut` once copied into the frame, allocated frame buffers returned from downstream (`dataReturnIn`) are deallocated via `bufferDeallocate`, and original data buffers returned by the helper (`bufferReturnIn`) go back upstream via `dataReturnOut`. Com status (`comStatusIn`) passes through unmodified to `comStatusOut`; additionally, when a frame is dropped (encryption failure or buffer-allocation failure), the framer emits a ready-for-more com status on `comStatusOut` so a `ComQueue`-driven downlink does not stall.

## Requirements

| Name | Description | Rationale | Validation |
|---|---|---|---|
| SVC-CCSDS-SDLS-FRAMER-001 | The CcsdsSdlsFramer shall accept data with frame context via the `Svc.Framer` interface (`dataIn`). | Standard framing pipeline entry point. | Unit test |
| SVC-CCSDS-SDLS-FRAMER-002 | The CcsdsSdlsFramer shall determine the security association (SA) index from the frame context when set, otherwise from the `SA_INDEX` parameter, record it in the frame context, and pass the data, SA index, and context to the encryption helper via `encryptOut`. | The SA index selects the encryption path; upstream components may override the configured default. | Unit test |
| SVC-CCSDS-SDLS-FRAMER-003 | Upon receiving encrypted data on `encryptIn`, the CcsdsSdlsFramer shall allocate a frame buffer via `bufferAllocate`, prepend the 16-bit SA index to the encrypted data, return ownership of the encrypted buffer via `encryptReturnOut`, and pass the resulting SDLS frame downstream via `dataOut`. | The SA index must lead the frame so the receiving deframer can extract it; prepending requires a new allocation. | Unit test |
| SVC-CCSDS-SDLS-FRAMER-004 | Upon a non-SUCCESS status passed forward on `encryptIn`, the CcsdsSdlsFramer shall emit the `EncryptionFailed` WARNING_HI event and return ownership of the accompanying buffer to the encryption subsystem via `encryptReturnOut`, and emit a ready-for-more com status on `comStatusOut`. | Encryption failures must be visible to the system, the buffer must not leak, and a ComQueue-driven downlink must not stall. | Unit test |
| SVC-CCSDS-SDLS-FRAMER-005 | The CcsdsSdlsFramer shall deallocate frame buffers received back on `dataReturnIn` via `bufferDeallocate`. | The framer allocated the frame buffer and must release it. | Unit test |
| SVC-CCSDS-SDLS-FRAMER-006 | The CcsdsSdlsFramer shall return original data buffers received back from the encryption helper (`bufferReturnIn`) upstream via `dataReturnOut`. | Original data buffers must return to their upstream allocator. | Unit test |
| SVC-CCSDS-SDLS-FRAMER-007 | The CcsdsSdlsFramer shall pass com status received on `comStatusIn` through to `comStatusOut` unmodified. | Ready signals must traverse the framing pipeline. | Unit test |
| SVC-CCSDS-SDLS-FRAMER-008 | Upon an invalid or undersized buffer allocation, the CcsdsSdlsFramer shall emit the `BufferAllocationFailed` WARNING_HI event, deallocate the undersized buffer when valid, return the encrypted buffer via `encryptReturnOut`, and emit a ready-for-more com status on `comStatusOut`. | Allocation failures must be reported, no buffer may leak, and a ComQueue-driven downlink must not stall; invalid buffers need not be deallocated. | Unit test |

## Design

The component is passive with no commands or telemetry. It composes two interfaces plus allocation ports:

| Kind | Name | Port Type | Description |
|---|---|---|---|
| sync input | dataIn | Svc.ComDataWithContext | Receives data to frame (from `Svc.Framer`). |
| output | dataOut | Svc.ComDataWithContext | Sends the SDLS frame downstream. |
| output | dataReturnOut | Svc.ComDataWithContext | Returns data buffer ownership upstream. |
| sync input | dataReturnIn | Svc.ComDataWithContext | Receives back ownership of frames sent on `dataOut`. |
| sync input | comStatusIn | Fw.SuccessCondition | Receives downstream ready status. |
| output | comStatusOut | Fw.SuccessCondition | Passes ready status upstream. |
| output | encryptOut | Svc.Ccsds.CcsdsSdlsEncryption | Sends the SA index and data to the encryption helper. |
| sync input | encryptIn | Svc.Ccsds.CcsdsSdlsData | Receives the operation status and encrypted data from the helper. |
| output | encryptReturnOut | Svc.ComDataWithContext | Returns ownership of encrypted buffers to the helper. |
| sync input | bufferReturnIn | Svc.ComDataWithContext | Receives back the data buffer sent on `encryptOut`. |
| output | bufferAllocate | Fw.BufferGet | Allocates the frame buffer for the SA prepend. |
| output | bufferDeallocate | Fw.BufferSend | Deallocates frame buffers. |

Events: `EncryptionFailed` (WARNING_HI, carries the `SdlsStatus`) and `BufferAllocationFailed` (WARNING_HI, carries the requested size as `FwSizeType`).

Parameters: `SA_INDEX` (U16, default 0) — the SA index used when the incoming frame context does not specify one (context `saIndex` equal to its default value of 0xFFFF is treated as unset).

## Configuration

The `SA_INDEX` parameter selects the default security association for downlink frames.

## Unit Testing

Rule-based testing (STest) with rules covering both SA selection paths, both error paths, the encrypted-data framing path, the ownership return paths, and the comStatus pass-through; a 10000-step randomized scenario interleaves all rules. Requirements are traced with `REQUIREMENT()` macros in the test main.

## See Also

- [`Svc/Ccsds/Interfaces/CcsdsSdlsEncrypt.fpp`](../../Interfaces/CcsdsSdlsEncrypt.fpp)
- [`Svc/Ccsds/CcsdsSdlsDeframer`](../../CcsdsSdlsDeframer/docs/sdd.md)
- [`Svc/Ccsds/SdlsSaRouter`](../../SdlsSaRouter/docs/sdd.md)
