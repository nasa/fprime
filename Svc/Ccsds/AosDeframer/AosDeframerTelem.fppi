        @ Telemetry for tracking received frames per Virtual Channel
        telemetry FrameCount: U32 \
            format "{} frames received"

        @ Telemetry for tracking extracted packets
        telemetry PacketCount: U32 \
            format "{} packets extracted"

        @ Telemetry for tracking Frame Error Control Field (FECF) errors (per physical channel)
        telemetry CrcErrorCount: U32 \
            format "{} FECF errors"
