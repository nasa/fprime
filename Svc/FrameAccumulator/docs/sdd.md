# Svc::FrameAccumulator

The `Svc::FrameAccumulator` component accumulates a stream of data (sequence of [Fw::Buffer](../../../Fw/Buffer/docs/sdd.md) objects) to extract full frames from.

The `Svc::FrameAccumulator` accepts as input a sequence of byte buffers, which typically come from a ground data system via a [ByteStreamDriver](../../../Drv/ByteStreamDriverModel/docs/sdd.md). It extracts the frames from the sequence of buffers and emits them on the `dataOut` output port.

## Internals

### Overview and configuration

The `Svc::FrameAccumulator` accumulates the [Fw::Buffer](../../../Fw/Buffer/docs/sdd.md) objects into a circular buffer ([Utils::CircularBuffer](../../../Utils/Types/CircularBuffer.hpp)). 

The component must be configured with a [`Svc::FrameDetector`](../FrameDetector.hpp) which is responsible for detecting frames in the circular buffer. An implementation of this for the F´ communications protocol is provided by `Svc::FrameDetectors::FprimeFrameDetector`.

The uplink frames need not be aligned on the buffer boundaries, and each frame may span one or more buffers.

`configure(..., crcWarningsEnabled)` enables CRC warnings by default. Pass `false` at startup to suppress warnings without stopping rejection or counting. Configure before processing input; this is not an in-flight command or parameter.

### Frame detection

The `Svc::FrameAccumulator` receives `Fw::Buffer` objects on its `dataIn` input port. These buffers are accumulated in a `Utils::CircularBuffer`. Every time a new buffer is accumulated into the circular buffer, the `Svc::FrameAccumulator` enters a loop to `detect()` a frame within the circular buffer, starting at the current head of the circular buffer. The `Svc::FrameDetector` returns one of four results:

- `NO_FRAME_DETECTED`: indicates no valid frame is present at the head of the circular buffer (for example, start word does not match the current head of the circular buffer). The `Svc::FrameAccumulator` rotates the circular buffer one byte and loops over to `detect()` again, or break the loop if the circular buffer is exhausted.
- `FRAME_DETECTED`: indicates there is a frame at the current head of the circular buffer. The `Svc::FrameAccumulator` allocates a new `Fw::Buffer` object to hold the frame, copies the detected frame from the circular buffer into the new `Fw::Buffer` object, and emits the new `Fw::Buffer` object (containing the frame) on its `dataOut` output port along with the `ComCfg::FrameContext` from the most recent `dataIn` call. The `Svc::FrameAccumulator` then rotates the circular buffer to remove the data that was just extracted, and deallocates the original `Fw::Buffer` that was received on the `dataIn` input port.
- `MORE_DATA_NEEDED`: indicates that more data is needed to determine whether there is a valid frame. The `Svc::FrameAccumulator` deallocates the original `Fw::Buffer` that was received on the `dataIn` input port and halts execution, effectively waiting for the next `Fw::Buffer` to be received on the `dataIn` input port.
- `CRC_FAILURE`: a complete candidate failed its CRC check. With `size_out = 0`, the accumulator discards one byte, counts the failure and requests a throttled warning if enabled, then resumes searching. Incomplete candidates, invalid lengths and ordinary non-frame data are not CRC failures.

`FprimeFrameDetector` reports this status; custom detectors may opt in. Detectors using only the original three results retain their existing names and numeric values. The CCSDS `CcsdsTcFrameDetector` still reports CRC mismatches as `NO_FRAME_DETECTED`: it does not increment the CRC-failure counter or emit `FrameCrcFailure`. Its actual search removals still contribute to the byte counter.

### CRC diagnostics and accounting

`FrameCrcFailure` is a `WARNING_LO` event with two cumulative `U64` arguments:

- `crcFailures`: complete CRC-rejected candidate checks, not unique physical packets. Overlapping candidates at different offsets count separately.
- `resyncBytesDiscarded`: bytes removed during non-frame, CRC-failed or oversized searches, not a count of bytes proven to have failed a CRC. Each rejection initially removes one byte, not the candidate's length. Forwarded frames and whole valid-frame allocation drops are excluded; drops already emit `FrameDetectionValidFrameDropped`.

Both counters start at zero per instance, saturate at `U64` maximum, and keep counting when warnings are disabled or throttled. Neither valid frames nor throttle re-arming reset them; no reset command or continuous telemetry is provided. Warning snapshots include the CRC-rejection byte but not later search removals.

Deployments override `Svc.FrameAccumulatorCfg` constants in `FrameAccumulatorCfg.fpp`:

- `CrcFailureWarningThrottle` (default `1`): positive warning quota.
- `CrcFailureWarningPeriodSeconds` (default `60`): positive period in seconds.

The FPP throttle period starts with the first warning. An exhausted quota re-arms on a later CRC failure at or after that period. The `timeCaller` source must advance: if it is unconnected or does not advance, the quota stays exhausted while counters continue. Valid frames do not re-arm it; counting and buffer handling do not depend on warning emission.

Diagnostic members are fixed-size and add no dynamic allocation after initialization. Existing output-buffer pool requests are unchanged. Processing is bounded by circular-buffer capacity, and each rejection removes one byte.

> [!NOTE]
> If the `Svc::FrameDetector` reports a `size_out` that is larger than the capacity of the internal accumulation buffer, the frame will cannot be processed. The `Svc::FrameAccumulator` logs a warning event, and continues searching for a new frame.


```mermaid
sequenceDiagram
    participant I as Input
    box Grey FrameAccumulator
    participant A as Accumulator
    participant D as Detector
    end

    I-->>A: Fw::Buffer
    activate A
    A-->A: Serialize into RingBuffer
    loop 
        A-->>D: detect()
        alt MORE_DATA_NEEDED
            A-->A: break
        else NO_FRAME_DETECTED
            A-->>A: ring.rotate(1)
        else CRC_FAILURE
            A-->>A: ring.rotate(1), increment counters
            A-->>A: throttled CRC warning if enabled
        else FRAME_DETECTED
            create participant Z as Output
            A-->>Z: Frame
        end
    end
    deactivate A
    destroy O
```

### Cleanup

The `cleanup()` method must be called to deallocate the memory used by the `Svc::FrameAccumulator` component safely before shutdown. This method deallocates the circular buffer that was set up during the `configure()` method.


## Usage Examples

The `Svc::FrameAccumulator` component is used in the uplink stack of many reference F´ application such as [the tutorials source code](https://github.com/fprime-community#tutorials). The below diagram shows the canonical configuration in which it is used.

![./img/deframer_uplink_stack.png](../../FprimeDeframer/docs/img/deframer_uplink_stack.png)

## Class Diagram

```mermaid
classDiagram
    class FrameAccumulator~PassiveComponent~ {
        + void configure(FrameDetector& detector, FwEnumStoreType allocationId, Fw::MemAllocator& allocator, FwSizeType store_size, bool crcWarningsEnabled = true)
        + void dataIn_handler(FwIndexType portNum, Fw::Buffer& recvBuffer, const Drv::ByteStreamStatus& recvStatus)
        + void processBuffer(Fw::Buffer& buffer, const ComCfg::FrameContext& context)
        + void processRing(const ComCfg::FrameContext& context)
    }
```

## Requirements

Requirement | Description | Rationale | Verification Method
----------- | ----------- | ----------| -------------------
SVC-FRAME-ACCUMULATOR-001 | `Svc::FrameAccumulator` shall accumulate a sequence of byte buffers until a full frame is received | FrameAccumulator is designed to re-assemble frames from sequence of bytes | Unit test |
SVC-FRAME-ACCUMULATOR-002 | `Svc::FrameAccumulator` shall detect once the accumulated buffers form a full frame and emit said frame | Pass frames to other parts of the system | Unit test |
SVC-FRAME-ACCUMULATOR-003 | `Svc::FrameAccumulator` shall accept byte buffers containing frames that are not aligned on a buffer boundary. | For flexibility, we do not require that the frames be aligned on a buffer boundary. | Unit test |
SVC-FRAME-ACCUMULATOR-004 | `Svc::FrameAccumulator` shall accept byte buffers containing frames that span one or more buffers. | For flexibility, we do not require each frame to fit in a single buffer. | Unit test |
SVC-FRAME-ACCUMULATOR-005 | A complete F Prime candidate with a CRC mismatch shall be reported distinctly; incomplete candidates and ordinary non-frame data shall not be CRC failures. | Diagnose actual checked CRC rejection. | Unit test; live GDS (see below) |
SVC-FRAME-ACCUMULATOR-006 | Enabled CRC warnings shall include cumulative failure and resynchronization-byte counts and obey a deployment-configurable positive quota and period. | Preserve fault visibility without flooding events. | Unit test; live GDS (see below) |
SVC-FRAME-ACCUMULATOR-007 | Failure and resynchronization-byte counters shall continue when warnings are disabled or throttled. | Event suppression shall not lose accounting. | Unit test; live GDS (see below) |
SVC-FRAME-ACCUMULATOR-008 | Resynchronization accounting shall count actual byte removal, preserve embedded/following valid frames, and exclude valid-frame allocation drops. | Avoid attributing an entire candidate's size to a one-byte rejection. | Unit test; live GDS (see below) |
SVC-FRAME-ACCUMULATOR-009 | Each instance shall initialize counts to zero and enable warnings by default with a startup disable option; valid frames and throttle re-arming shall not reset counts. | Deliberate, independent instance state. | Unit test; live GDS (see below) |
SVC-FRAME-ACCUMULATOR-010 | Diagnostic counters shall saturate at U64 maximum; diagnostic processing shall remain bounded and require no post-initialization dynamic allocation. | Counter rollover and corruption shall not impede recovery. | Unit test and inspection |

## Integration Testing

The reusable [`test/int/`](../test/int/README.md) scenario uses a live GDS and a
raw-uplink hook. It checks partial input, warning severity/count snapshots, default
throttling, continued counts and command recovery. Later snapshots confirm counts
survive valid frames and throttle re-arming.

The scenario requires the default warning configuration. Unit tests cover disabled
warnings, alternate quotas/periods, embedded-frame recovery, allocation exhaustion,
independent instances and saturation. No hardware or flight validation is implied.

## Port Descriptions

| Kind | Name | Type | Description |
|---|---|---|---|
| `guarded input` | dataIn | `Svc.ComDataWithContext` | Receives a stream of byte buffers from a [Communication Adapter](../../Interfaces/docs/sdd.md) |
| `output` | dataOut | `Svc.ComDataWithContext` | Port for sending an extracted frame out |
| `output` | bufferAllocate | `Fw.BufferGet` | Port for allocating buffer to hold extracted frame |
| `output`| bufferDeallocate | `Fw.BufferSend` | Port for deallocating buffers received on dataIn. |
| `output` | dataReturnOut | `Svc.ComDataWithContext` | Port for returning ownership of buffers received on dataIn |
| `sync input` | dataReturnIn | `Svc.ComDataWithContext` | Receiving back ownership of buffers sent on dataOut |
