# Svc::Ccsds::AsmFramer

The `Svc::Ccsds::AsmFramer` is an implementation of the [FramerInterface](../../../Interfaces/docs/sdd.md) that prepends the CCSDS Attached Sync Marker (ASM) to transfer frames, producing Sync-Marked Transfer Frames (SMTFs) as specified by [CCSDS 131.0-B-5, TM Synchronization and Channel Coding](https://ccsds.org/Pubs/131x0b5.pdf), Section 9.

It receives a transfer frame (e.g. a TM or AOS Transfer Frame) on its input port and produces the same frame preceded by the ASM on its output port. The frame content is not modified: per the standard, the ASM is not part of the transfer frame; it is a marker attached ahead of the data it delimits (Standard 9.4) so the receiving end can acquire frame synchronization (Standard 9.2).

The `Svc::Ccsds::AsmFramer` is designed to chain downstream of a transfer frame framer such as [`Svc::Ccsds::TmFramer`](../../TmFramer/docs/sdd.md) or [`Svc::Ccsds::AosFramer`](../../AosFramer/docs/sdd.md), and upstream of a Communications Adapter such as [`Svc::ComStub`](../../../ComStub/docs/sdd.md).

## ASM Patterns

By default the AsmFramer uses the 32-bit ASM pattern `0x1ACFFC1D`, which the standard specifies for uncoded, convolutionally coded, Reed-Solomon, concatenated, rate-7/8 Transfer-Frame LDPC, and SMTF-stream LDPC coded data (Standard 9.3.1, figure 9-1).

Projects using other channel coding may configure the appropriate pattern at initialization via `configure()`, up to 128 bits (Standard 9.3.2–9.3.4), e.g.:

```cpp
// Rate-1/2 Turbo and rates 1/2, 2/3, 4/5 Transfer-Frame LDPC (Standard figure 9-2)
static const U8 turboAsm[] = {0x03, 0x47, 0x76, 0xC7, 0x27, 0x28, 0x95, 0xB0};
asmFramer.configure(turboAsm, sizeof(turboAsm));
```

## Internals

The `Svc::Ccsds::AsmFramer` uses an internal (member) buffer sized to hold the largest supported ASM plus a frame of `ComCfg::TmFrameFixedSize` bytes. The buffer **must** be returned to the AsmFramer via the `dataReturnIn` port once it has been used or consumed. Should a component want to use the frame data past the time it is returned, the data should be copied before the buffer is returned via `dataReturnIn`.

## Usage Example

Projects insert an `AsmFramer` instance between the transfer-frame layer and the com interface, for example with the `ComCcsds` split subtopologies:

```fpp
ComCcsds.TmTcFraming.framedDataOut -> asmFramer.dataIn
asmFramer.dataReturnOut            -> ComCcsds.TmTcFraming.framedDataReturnIn
asmFramer.dataOut                  -> comStub.dataIn
comStub.dataReturnOut              -> asmFramer.dataReturnIn
comStub.comStatusOut               -> asmFramer.comStatusIn
asmFramer.comStatusOut             -> ComCcsds.TmTcFraming.framedComStatusIn
```

On the ground side, the matching `fprime-gds` deframing chain is selected with
`--framing-selection space-packet-space-data-link-asm` (or `ccsds-asm` for the ASM layer alone).

## Port Descriptions

| Kind | Name | Port Type | Description |
|---|---|---|---|
| sync input | dataIn | Svc.ComDataWithContext | Receives a transfer frame to sync-mark, in a Fw::Buffer with optional context |
| output | dataOut | Svc.ComDataWithContext | Outputs the Sync-Marked Transfer Frame (ASM + frame) |
| output | dataReturnOut | Svc.ComDataWithContext | Returns ownership of the incoming Fw::Buffer to its sender once handled |
| sync input | dataReturnIn | Svc.ComDataWithContext | Receives back ownership of the emitted SMTF buffer |
| sync input | comStatusIn | Fw.SuccessCondition | Receives status from the downstream communication adapter |
| output | comStatusOut | Fw.SuccessCondition | Passes status through to the upstream framer per the [Framer Status Protocol](../../../../docs/reference/communication-adapter-interface.md#framer-status-protocol) |

## Requirements

| Name | Description | Validation |
|---|---|---|
| SVC-CCSDS-ASM-FRAMER-001 | The AsmFramer shall implement the `Svc.FramerInterface`. | Inspection, Unit Test |
| SVC-CCSDS-ASM-FRAMER-002 | The AsmFramer shall prepend an Attached Sync Marker to each frame received on `dataIn` and emit the resulting Sync-Marked Transfer Frame on `dataOut`, per CCSDS 131.0-B-5 Section 9.4. | Unit Test |
| SVC-CCSDS-ASM-FRAMER-003 | The AsmFramer shall default to the 32-bit ASM pattern `0x1ACFFC1D` (CCSDS 131.0-B-5 Section 9.3.1). | Unit Test, Inspection |
| SVC-CCSDS-ASM-FRAMER-004 | The AsmFramer shall allow projects to configure the ASM pattern and length (up to 128 bits) at initialization to support the coded-data ASMs of CCSDS 131.0-B-5 Sections 9.3.2–9.3.4. | Unit Test |
| SVC-CCSDS-ASM-FRAMER-005 | The AsmFramer shall not alter, reorder, or drop payload frame data. | Unit Test |
| SVC-CCSDS-ASM-FRAMER-006 | The AsmFramer shall return ownership of the input buffer via `dataReturnOut` after framing is complete. | Unit Test |
| SVC-CCSDS-ASM-FRAMER-007 | The AsmFramer shall use an internal frame buffer and shall only accept new data when it owns that buffer; the buffer is returned via `dataReturnIn`. | Unit Test |
| SVC-CCSDS-ASM-FRAMER-008 | The AsmFramer shall pass communication status from `comStatusIn` through to `comStatusOut` per the Framer Status Protocol. | Unit Test, Integration Test |
| SVC-CCSDS-ASM-FRAMER-009 | The AsmFramer shall assert if input data exceeds the internal buffer capacity. | Unit Test |
