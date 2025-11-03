module ComFprime {

    enum Ports_ComPacketQueue : U8 {
        EVENTS,
        TELEMETRY,
    };

    enum Ports_ComBufferQueue : U8 {
        FILE
    };

    # ----------------------------------------------------------------------
    # Active Components
    # ----------------------------------------------------------------------
    instance comQueue: Svc.ComQueue base id ComFprimeConfig.BASE_ID + 0x00000 \
        queue size ComFprimeConfig.QueueSizes.comQueue \
        stack size ComFprimeConfig.StackSizes.comQueue \
        priority ComFprimeConfig.Priorities.comQueue \
    {
        phase Fpp.ToCpp.Phases.configComponents """
        using namespace ComFprime;
        Svc::ComQueue::QueueConfigurationTable configurationTable;
        
        // Events (highest-priority)
        configurationTable.entries[Ports_ComPacketQueue::EVENTS].depth = ComFprimeConfig::QueueDepths::events;
        configurationTable.entries[Ports_ComPacketQueue::EVENTS].priority = ComFprimeConfig::QueuePriorities::events;
        // Telemetry
        configurationTable.entries[Ports_ComPacketQueue::TELEMETRY].depth = ComFprimeConfig::QueueDepths::tlm;
        configurationTable.entries[Ports_ComPacketQueue::TELEMETRY].priority = ComFprimeConfig::QueuePriorities::tlm;
        // File Downlink Queue
        configurationTable.entries[Ports_ComPacketQueue::NUM_CONSTANTS + Ports_ComBufferQueue::FILE].depth = ComFprimeConfig::QueueDepths::file;
        configurationTable.entries[Ports_ComPacketQueue::NUM_CONSTANTS + Ports_ComBufferQueue::FILE].priority = ComFprimeConfig::QueuePriorities::file;
        // Allocation identifier is 0 as the MallocAllocator discards it
        ComFprime::comQueue.configure(configurationTable, 0, ComFprime::Allocation::memAllocator);
        """
        phase Fpp.ToCpp.Phases.tearDownComponents """
        ComFprime::comQueue.cleanup();
        """
    }

    # ----------------------------------------------------------------------
    # Passive Components
    # ----------------------------------------------------------------------
    instance frameAccumulator: Svc.FrameAccumulator base id ComFprimeConfig.BASE_ID + 0x01000 \ 
    {
        phase Fpp.ToCpp.Phases.configObjects """
        Svc::FrameDetectors::FprimeFrameDetector frameDetector;
        """
        
        phase Fpp.ToCpp.Phases.configComponents """
        ComFprime::frameAccumulator.configure(
            ConfigObjects::ComFprime_frameAccumulator::frameDetector,
            1,
            ComFprime::Allocation::memAllocator,
            ComFprimeConfig::BuffMgr::frameAccumulatorSize
        );
        """

        phase Fpp.ToCpp.Phases.tearDownComponents """
        ComFprime::frameAccumulator.cleanup();
        """
    }

    instance commsBufferManager: Svc.BufferManager base id ComFprimeConfig.BASE_ID + 0x02000 \
    {
        phase Fpp.ToCpp.Phases.configObjects """
        Svc::BufferManager::BufferBins bins;
        """
        
        phase Fpp.ToCpp.Phases.configComponents """
        memset(&ConfigObjects::ComFprime_commsBufferManager::bins, 0, sizeof(ConfigObjects::ComFprime_commsBufferManager::bins));
        ConfigObjects::ComFprime_commsBufferManager::bins.bins[0].bufferSize = ComFprimeConfig::BuffMgr::commsBuffSize;
        ConfigObjects::ComFprime_commsBufferManager::bins.bins[0].numBuffers = ComFprimeConfig::BuffMgr::commsBuffCount;
        ConfigObjects::ComFprime_commsBufferManager::bins.bins[1].bufferSize = ComFprimeConfig::BuffMgr::commsFileBuffSize;
        ConfigObjects::ComFprime_commsBufferManager::bins.bins[1].numBuffers = ComFprimeConfig::BuffMgr::commsFileBuffCount;
        ComFprime::commsBufferManager.setup(
            ComFprimeConfig::BuffMgr::commsBuffMgrId,
            0,
            ComFprime::Allocation::memAllocator,
            ConfigObjects::ComFprime_commsBufferManager::bins
        );
        """

        phase Fpp.ToCpp.Phases.tearDownComponents """
        ComFprime::commsBufferManager.cleanup();
        """
    }

    instance deframer: Svc.FprimeDeframer base id ComFprimeConfig.BASE_ID + 0x03000

    instance framer: Svc.FprimeFramer base id ComFprimeConfig.BASE_ID + 0x04000

    instance fprimeRouter: Svc.FprimeRouter base id ComFprimeConfig.BASE_ID + 0x05000

    instance comStub: Svc.ComStub base id ComFprimeConfig.BASE_ID + 0x06000

    topology FramingSubtopology {
        # Usage Note:
        #
        # When importing this subtopology, users shall establish 5 port connections with a component implementing
        # the Svc.Com (Svc/Interfaces/Com.fpp) interface. They are as follows:
        #
        # 1) Outputs:
        #     - ComFprime.framer.dataOut                 -> [Svc.Com].dataIn
        #     - ComFprime.frameAccumulator.dataReturnOut -> [Svc.Com].dataReturnIn
        # 2) Inputs:
        #     - [Svc.Com].dataReturnOut -> ComFprime.framer.dataReturnIn
        #     - [Svc.Com].comStatusOut  -> ComFprime.framer.comStatusIn
        #     - [Svc.Com].dataOut       -> ComFprime.frameAccumulator.dataIn

        # Active Components
        instance comQueue

        # Passive Components
        instance commsBufferManager
        instance frameAccumulator
        instance deframer
        instance framer
        instance fprimeRouter

        connections Downlink {
            # ComQueue <-> Framer
            comQueue.dataOut     -> framer.dataIn
            framer.dataReturnOut -> comQueue.dataReturnIn
            # Buffer Management for Framer
            framer.bufferAllocate   -> commsBufferManager.bufferGetCallee
            framer.bufferDeallocate -> commsBufferManager.bufferSendIn
            # ComStatus passback
            framer.comStatusOut  -> comQueue.comStatusIn
            # (Outgoing) Framer <-> ComInterface connections shall be established by the user
        }

        connections Uplink {
            # (Incoming) ComInterface <-> FrameAccumulator connections shall be established by the user
            # FrameAccumulator buffer allocations
            frameAccumulator.bufferDeallocate -> commsBufferManager.bufferSendIn
            frameAccumulator.bufferAllocate   -> commsBufferManager.bufferGetCallee
            # FrameAccumulator <-> Deframer
            frameAccumulator.dataOut -> deframer.dataIn
            deframer.dataReturnOut   -> frameAccumulator.dataReturnIn
            # Deframer <-> Router
            deframer.dataOut           -> fprimeRouter.dataIn
            fprimeRouter.dataReturnOut -> deframer.dataReturnIn
            # Router buffer allocations
            fprimeRouter.bufferAllocate   -> commsBufferManager.bufferGetCallee
            fprimeRouter.bufferDeallocate -> commsBufferManager.bufferSendIn
        }
    } # end FramingSubtopology


    # This subtopology uses FramingSubtopology with a ComStub component for Com Interface
    topology Subtopology {
        import FramingSubtopology

        instance comStub

        connections ComStub {
            # Framer <-> ComStub (Downlink)
            ComFprime.framer.dataOut -> comStub.dataIn
            comStub.dataReturnOut    -> ComFprime.framer.dataReturnIn
            comStub.comStatusOut     -> ComFprime.framer.comStatusIn

            # ComStub <-> FrameAccumulator (Uplink)
            comStub.dataOut -> ComFprime.frameAccumulator.dataIn
            ComFprime.frameAccumulator.dataReturnOut -> comStub.dataReturnIn
        }
    } # end Subtopology

} # end ComFprime
