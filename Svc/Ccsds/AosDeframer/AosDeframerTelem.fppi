        @ Latest VC Frame Recieved from header
        telemetry LatestVcFrameCount: VCxU32

        @ Processed frames per Virtual Channel
        telemetry FramesProcessed: VCxU32 \
            format "{} frames processed"

        @ Packets extracted per Virtual Channel
        telemetry PacketsExtracted: VCxU32 \
            format "{} packets extracted"

        @ Frame Error Control Field (FECF) errors (per physical channel)
        telemetry CrcErrorCount: U32 \
            format "{} FECF errors"
