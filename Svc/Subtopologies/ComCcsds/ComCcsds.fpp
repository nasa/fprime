module ComCcsds {

    # ComPacket Queue enum for queue types
    enum Ports_ComPacketQueue {
        EVENTS,
        TELEMETRY,
        FILE_QUEUE 
    }

    # ----------------------------------------------------------------------
    # Active Components
    # ----------------------------------------------------------------------
    instance comQueue: Svc.ComQueue base id ComCcsdsConfig.BASE_ID + 0x000000 \
        queue size ComCcsdsConfig.QueueSizes.comQueue \
        stack size ComCcsdsConfig.StackSizes.comQueue \
        priority ComCcsdsConfig.Priorities.comQueue \
    {
        phase Fpp.ToCpp.Phases.configConstants """
        enum {
            EVENTS,
            TELEMETRY,
            FILE_QUEUE
        };
        """
        phase Fpp.ToCpp.Phases.configComponents """
        Svc::ComQueue::QueueConfigurationTable configurationTable;

        // Events (highest-priority)
        configurationTable.entries[ConfigConstants::ComCcsds_comQueue::EVENTS].depth = ComCcsdsConfig::QueueDepths::events;
        configurationTable.entries[ConfigConstants::ComCcsds_comQueue::EVENTS].priority = ComCcsdsConfig::QueuePriorities::events;

        // Telemetry
        configurationTable.entries[ConfigConstants::ComCcsds_comQueue::TELEMETRY].depth = ComCcsdsConfig::QueueDepths::tlm;
        configurationTable.entries[ConfigConstants::ComCcsds_comQueue::TELEMETRY].priority = ComCcsdsConfig::QueuePriorities::tlm;

        // File Downlink Queue
        configurationTable.entries[ConfigConstants::ComCcsds_comQueue::FILE_QUEUE].depth = ComCcsdsConfig::QueueDepths::file;
        configurationTable.entries[ConfigConstants::ComCcsds_comQueue::FILE_QUEUE].priority = ComCcsdsConfig::QueuePriorities::file;

        // Allocation identifier is 0 as the MallocAllocator discards it
        ComCcsds::comQueue.configure(configurationTable, 0, ComCcsds::Allocation::memAllocator);
        """
        phase Fpp.ToCpp.Phases.tearDownComponents """
        ComCcsds::comQueue.cleanup();
        """
    }

    # ----------------------------------------------------------------------
    # Passive Components
    # ----------------------------------------------------------------------
    instance frameAccumulator: Svc.FrameAccumulator base id ComCcsdsConfig.BASE_ID + 0x001000 \ 
    {

        phase Fpp.ToCpp.Phases.configObjects """
        Svc::FrameDetectors::CcsdsTcFrameDetector frameDetector;
        """
        phase Fpp.ToCpp.Phases.configComponents """
        ComCcsds::frameAccumulator.configure(
            ConfigObjects::ComCcsds_frameAccumulator::frameDetector,
            1,
            ComCcsds::Allocation::memAllocator,
            ComCcsdsConfig::BuffMgr::frameAccumulatorSize
        );
        """

        phase Fpp.ToCpp.Phases.tearDownComponents """
        ComCcsds::frameAccumulator.cleanup();
        """
    }

    instance commsBufferManager: Svc.BufferManager base id ComCcsdsConfig.BASE_ID + 0x002000 \
    {
        phase Fpp.ToCpp.Phases.configObjects """
        Svc::BufferManager::BufferBins bins;
        """

        phase Fpp.ToCpp.Phases.configComponents """
        memset(&ConfigObjects::ComCcsds_commsBufferManager::bins, 0, sizeof(ConfigObjects::ComCcsds_commsBufferManager::bins));
        ConfigObjects::ComCcsds_commsBufferManager::bins.bins[0].bufferSize = ComCcsdsConfig::BuffMgr::commsBuffSize;
        ConfigObjects::ComCcsds_commsBufferManager::bins.bins[0].numBuffers = ComCcsdsConfig::BuffMgr::commsBuffCount;
        ConfigObjects::ComCcsds_commsBufferManager::bins.bins[1].bufferSize = ComCcsdsConfig::BuffMgr::commsFileBuffSize;
        ConfigObjects::ComCcsds_commsBufferManager::bins.bins[1].numBuffers = ComCcsdsConfig::BuffMgr::commsFileBuffCount;
        ComCcsds::commsBufferManager.setup(
            ComCcsdsConfig::BuffMgr::commsBuffMgrId,
            0,
            ComCcsds::Allocation::memAllocator,
            ConfigObjects::ComCcsds_commsBufferManager::bins
        );
        """

        phase Fpp.ToCpp.Phases.tearDownComponents """
        ComCcsds::commsBufferManager.cleanup();
        """
    }

    instance fprimeRouter: Svc.FprimeRouter base id ComCcsdsConfig.BASE_ID + 0x003000 \
    
    instance comStub: Svc.ComStub base id ComCcsdsConfig.BASE_ID + 0x004000 \

    instance tcDeframer: Svc.Ccsds.TcDeframer base id ComCcsdsConfig.BASE_ID + 0x005000 \

    instance spacePacketDeframer: Svc.Ccsds.SpacePacketDeframer base id ComCcsdsConfig.BASE_ID + 0x006000 \

    instance tmFramer: Svc.Ccsds.TmFramer base id ComCcsdsConfig.BASE_ID + 0x007000 \

    instance spacePacketFramer: Svc.Ccsds.SpacePacketFramer base id ComCcsdsConfig.BASE_ID + 0x008000 \

    instance apidManager: Svc.Ccsds.ApidManager base id ComCcsdsConfig.BASE_ID + 0x009000 \

    topology Subtopology {
        # Active Components
        instance comQueue

        # Passive Components
        instance commsBufferManager
        instance frameAccumulator
        instance fprimeRouter
        instance comStub
        instance tcDeframer
        instance spacePacketDeframer
        instance tmFramer
        instance spacePacketFramer
        instance apidManager

        connections Downlink {


            # ComQueue <-> SpacePacketFramer
            comQueue.dataOut                -> spacePacketFramer.dataIn
            spacePacketFramer.dataReturnOut -> comQueue.dataReturnIn
            # SpacePacketFramer buffer and APID management
            spacePacketFramer.bufferAllocate   -> commsBufferManager.bufferGetCallee
            spacePacketFramer.bufferDeallocate -> commsBufferManager.bufferSendIn
            spacePacketFramer.getApidSeqCount  -> apidManager.getApidSeqCountIn
            # SpacePacketFramer <-> TmFramer
            spacePacketFramer.dataOut -> tmFramer.dataIn
            tmFramer.dataReturnOut    -> spacePacketFramer.dataReturnIn
            # Framer <-> ComStub
            tmFramer.dataOut      -> comStub.dataIn
            comStub.dataReturnOut -> tmFramer.dataReturnIn
            # ComStatus
            comStub.comStatusOut            -> tmFramer.comStatusIn
            tmFramer.comStatusOut           -> spacePacketFramer.comStatusIn
            spacePacketFramer.comStatusOut  -> comQueue.comStatusIn
        }

        connections Uplink {
            # ComStub <-> FrameAccumulator
            comStub.dataOut                -> frameAccumulator.dataIn
            frameAccumulator.dataReturnOut -> comStub.dataReturnIn
            # FrameAccumulator buffer allocations
            frameAccumulator.bufferDeallocate -> commsBufferManager.bufferSendIn
            frameAccumulator.bufferAllocate   -> commsBufferManager.bufferGetCallee
            # FrameAccumulator <-> Deframer
            frameAccumulator.dataOut          -> tcDeframer.dataIn
            tcDeframer.dataReturnOut          -> frameAccumulator.dataReturnIn
            # TcDeframer <-> SpacePacketDeframer
            tcDeframer.dataOut                -> spacePacketDeframer.dataIn
            spacePacketDeframer.dataReturnOut -> tcDeframer.dataReturnIn
            # SpacePacketDeframer APID validation
            spacePacketDeframer.validateApidSeqCount -> apidManager.validateApidSeqCountIn
            # SpacePacketDeframer <-> Router
            spacePacketDeframer.dataOut -> fprimeRouter.dataIn
            fprimeRouter.dataReturnOut  -> spacePacketDeframer.dataReturnIn
            # Router buffer allocations
            fprimeRouter.bufferAllocate   -> commsBufferManager.bufferGetCallee
            fprimeRouter.bufferDeallocate -> commsBufferManager.bufferSendIn
         
        }

    } # end topology
} # end ComCcsds Subtopology
