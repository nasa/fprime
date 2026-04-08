        @ Latest VC Frame Received from header
        telemetry LatestVcFrameCount: U32

        @ Processed frames per Virtual Channel
        telemetry FramesProcessed: U32 \
            format "{} frames processed"

        @ Packets extracted per Virtual Channel
        telemetry PacketsExtracted: U32 \
            format "{} packets extracted"

        @ Frame Error Control Field (FECF) errors (per physical channel)
        telemetry CrcErrorCount: U32 \
            format "{} FECF errors"
