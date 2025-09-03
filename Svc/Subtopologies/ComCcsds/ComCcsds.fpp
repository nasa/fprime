module ComCcsds {

# Usage Note:
#
# When importing this subtopology, users shall establish 5 port connections with a component implementing
# the Svc.Com (Svc/Interfaces/Com.fpp) interface. They are as follows:
#
# 1) Outputs:
#     - ComCcsds.framer.dataOut                 -> [Svc.Com].dataIn
#     - ComCcsds.frameAccumulator.dataReturnOut -> [Svc.Com].dataReturnIn
# 2) Inputs:
#     - [Svc.Com].dataReturnOut -> ComCcsds.framer.dataReturnIn
#     - [Svc.Com].comStatusOut  -> ComCcsds.framer.comStatusIn
#     - [Svc.Com].dataOut       -> ComCcsds.frameAccumulator.dataIn

    # TODO: check all base IDs
    instance comStub: Svc.ComStub base id 0x10020000

    topology Subtopology {
        import FramingCcsds.Subtopology

        # Passively
        instance comStub

        connections ComStub {
            # Framer <-> ComStub (Downlink)
            FramingCcsds.framer.dataOut  -> comStub.dataIn
            comStub.dataReturnOut -> FramingCcsds.framer.dataReturnIn
            comStub.comStatusOut       -> FramingCcsds.framer.comStatusIn

            # ComStub <-> FrameAccumulator (Uplink)
            comStub.dataOut                -> FramingCcsds.frameAccumulator.dataIn
            FramingCcsds.frameAccumulator.dataReturnOut -> comStub.dataReturnIn
        }

    } # end topology
} # end ComCcsds Subtopology
