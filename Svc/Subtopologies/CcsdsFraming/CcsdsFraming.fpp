module CcsdsFraming {

    # ----------------------------------------------------------------------
    # Passive Components
    # ----------------------------------------------------------------------

    # NOTE: name 'framer' is used for the framer that connects to the Com Adapter Interface for better subtopology interoperability
    instance framer: Svc.Ccsds.TmFramer base id CcsdsFramingConfig.BASE_ID + 0x00000

    instance tcDeframer: Svc.Ccsds.TcDeframer base id CcsdsFramingConfig.BASE_ID + 0x01000

    instance frameAccumulator: Svc.FrameAccumulator base id CcsdsFramingConfig.BASE_ID + 0x02000 \
    {
        phase Fpp.ToCpp.Phases.configObjects """
        Svc::FrameDetectors::CcsdsTcFrameDetector frameDetector;
        """
        phase Fpp.ToCpp.Phases.configComponents """
        CcsdsFraming::frameAccumulator.configure(
            ConfigObjects::CcsdsFraming_frameAccumulator::frameDetector,
            1,
            CcsdsFraming::Allocation::memAllocator,
            CcsdsFramingConfig::BuffMgr::frameAccumulatorSize
        );
        """

        phase Fpp.ToCpp.Phases.tearDownComponents """
        CcsdsFraming::frameAccumulator.cleanup();
        """
    }

    topology Subtopology {
        # Usage Note:
        #
        # This subtopology boxes the CCSDS TM/TC transfer frame layer: the TM framer (downlink),
        # and the frame accumulator + TC deframer (uplink). When importing this subtopology, users
        # shall establish the following external connections:
        #
        # 1) Upstream (packet layer, e.g. the SpacePacket subtopology):
        #     - [upstream].dataOut                  -> CcsdsFraming.framer.dataIn
        #     - CcsdsFraming.framer.dataReturnOut   -> [upstream].dataReturnIn
        #     - CcsdsFraming.framer.comStatusOut    -> [upstream].comStatusIn
        #     - CcsdsFraming.tcDeframer.dataOut     -> [upstream].dataIn (deframed data)
        #     - [upstream].dataReturnOut            -> CcsdsFraming.tcDeframer.dataReturnIn
        # 2) Downstream (a component implementing the Svc.Com interface):
        #     - CcsdsFraming.framer.dataOut                 -> [Svc.Com].dataIn
        #     - CcsdsFraming.frameAccumulator.dataReturnOut -> [Svc.Com].dataReturnIn
        #     - [Svc.Com].dataReturnOut -> CcsdsFraming.framer.dataReturnIn
        #     - [Svc.Com].comStatusOut  -> CcsdsFraming.framer.comStatusIn
        #     - [Svc.Com].dataOut       -> CcsdsFraming.frameAccumulator.dataIn
        # 3) Buffer management (e.g. a Svc.BufferManager):
        #     - CcsdsFraming.frameAccumulator.bufferAllocate   -> [BufferManager].bufferGetCallee
        #     - CcsdsFraming.frameAccumulator.bufferDeallocate -> [BufferManager].bufferSendIn

        instance framer
        instance tcDeframer
        instance frameAccumulator

        connections Uplink {
            # FrameAccumulator <-> TcDeframer
            frameAccumulator.dataOut -> tcDeframer.dataIn
            tcDeframer.dataReturnOut -> frameAccumulator.dataReturnIn
        }
    } # end Subtopology

} # end CcsdsFraming
