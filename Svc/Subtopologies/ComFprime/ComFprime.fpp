module ComFprime {

    enum Ports_ComPacketQueue {
        EVENTS,
        TELEMETRY,
        FILE_QUEUE 
    };

    # ----------------------------------------------------------------------
    # Active Components
    # ----------------------------------------------------------------------
    instance comQueue: Svc.ComQueue base id ComFprimeConfig.BASE_ID + 0x0100 \
        queue size ComFprimeConfig.QueueSizes.comQueue \
        stack size ComFprimeConfig.StackSizes.comQueue \
        priority ComFprimeConfig.Priorities.comQueue \
    {
        phase Fpp.ToCpp.Phases.configConstants """
        enum{
            EVENTS,
            TELEMETRY,
            FILE_QUEUE
        };
        """
        phase Fpp.ToCpp.Phases.configComponents """
        Svc::ComQueue::QueueConfigurationTable configurationTable;
        // Events (highest-priority)
        configurationTable.entries[ConfigConstants::ComFprime_comQueue::EVENTS].depth = ComFprimeConfig::QueueDepths::events;
        configurationTable.entries[ConfigConstants::ComFprime_comQueue::EVENTS].priority = ComFprimeConfig::QueuePriorities::events;
        // Telemetry
        configurationTable.entries[ConfigConstants::ComFprime_comQueue::TELEMETRY].depth = ComFprimeConfig::QueueDepths::tlm;
        configurationTable.entries[ConfigConstants::ComFprime_comQueue::TELEMETRY].priority = ComFprimeConfig::QueuePriorities::tlm;
        // File Downlink Queue
        configurationTable.entries[ConfigConstants::ComFprime_comQueue::FILE_QUEUE].depth = ComFprimeConfig::QueueDepths::file;
        configurationTable.entries[ConfigConstants::ComFprime_comQueue::FILE_QUEUE].priority = ComFprimeConfig::QueuePriorities::file;
        // Allocation identifier is 0 as the MallocAllocator discards it
        ComFprime::comQueue.configure(configurationTable, 0, ComFprime::Allocation::memAllocator);
        """
    }

    instance cmdSeq: Svc.CmdSequencer base id ComFprimeConfig.BASE_ID + 0x0200 \
        queue size ComFprimeConfig.QueueSizes.cmdSeq \
        stack size ComFprimeConfig.StackSizes.cmdSeq \
        priority ComFprimeConfig.Priorities.cmdSeq \
    {
        phase Fpp.ToCpp.Phases.configComponents """
        ComFprime::cmdSeq.allocateBuffer(0, ComFprime::Allocation::memAllocator, ComFprimeConfig::BuffMgr::cmdSeqBuffSize);
        """

        phase Fpp.ToCpp.Phases.tearDownComponents """
        ComFprime::cmdSeq.deallocateBuffer(ComFprime::Allocation::memAllocator);
        """
    }

    # ----------------------------------------------------------------------
    # Passive Components
    # ----------------------------------------------------------------------
    instance frameAccumulator: Svc.FrameAccumulator base id ComFprimeConfig.BASE_ID + 0x0500 \ 
    {

        phase Fpp.ToCpp.Phases.configComponents """
        ComFprime::frameAccumulator.configure(
            ComFprime::Detector::frameDetector,
            1,
            ComFprime::Allocation::memAllocator,
            ComFprimeConfig::BuffMgr::frameAccumulatorSize
        );
        """

        phase Fpp.ToCpp.Phases.tearDownComponents """
        ComFprime::frameAccumulator.cleanup();
        """
    }

    instance commsBufferManager: Svc.BufferManager base id ComFprimeConfig.BASE_ID + 0x0600 \
    {
        phase Fpp.ToCpp.Phases.configComponents """
        memset(&ComFprime::BufferManagerBins::bins, 0, sizeof(ComFprime::BufferManagerBins::bins));
        ComFprime::BufferManagerBins::bins.bins[0].bufferSize = ComFprimeConfig::BuffMgr::commsBuffSize;
        ComFprime::BufferManagerBins::bins.bins[0].numBuffers = ComFprimeConfig::BuffMgr::commsBuffCount;
        ComFprime::BufferManagerBins::bins.bins[1].bufferSize = ComFprimeConfig::BuffMgr::commsFileBuffSize;
        ComFprime::BufferManagerBins::bins.bins[1].numBuffers = ComFprimeConfig::BuffMgr::commsFileBuffCount;
        ComFprime::commsBufferManager.setup(
            ComFprimeConfig::BuffMgr::commsBuffMgrId,
            0,
            ComFprime::Allocation::memAllocator,
            ComFprime::BufferManagerBins::bins
        );
        """

        phase Fpp.ToCpp.Phases.tearDownComponents """
        ComFprime::commsBufferManager.cleanup();
        """
    }

    instance deframer: Svc.FprimeDeframer base id ComFprimeConfig.BASE_ID + 0x0700 \

    instance fprimeFramer: Svc.FprimeFramer base id ComFprimeConfig.BASE_ID + 0x0800 \

    instance fprimeRouter: Svc.FprimeRouter base id ComFprimeConfig.BASE_ID + 0x0900 \
    
    instance comStub: Svc.ComStub base id ComFprimeConfig.BASE_ID + 0x0A00 \

    topology Subtopology {
        # Active Components
        instance comQueue
        instance cmdSeq

        # Passive Components
        instance commsBufferManager
        instance frameAccumulator
        instance deframer
        instance fprimeFramer
        instance fprimeRouter
        instance comStub
        instance comDriver


        connections Downlink {
            # Inputs to ComQueue (events, telemetry)
            # ComQueue <-> Framer
            comQueue.dataOut           -> fprimeFramer.dataIn
            fprimeFramer.dataReturnOut -> comQueue.dataReturnIn
            # Buffer Management for Framer
            fprimeFramer.bufferAllocate   -> commsBufferManager.bufferGetCallee
            fprimeFramer.bufferDeallocate -> commsBufferManager.bufferSendIn
            # Framer <-> ComStub
            fprimeFramer.dataOut  -> comStub.dataIn
            comStub.dataReturnOut -> fprimeFramer.dataReturnIn
            # ComStub <-> ComDriver
            comStub.drvSendOut      -> comDriver.$send
            comDriver.sendReturnOut -> comStub.drvSendReturnIn
            comDriver.ready         -> comStub.drvConnected
            # ComStatus
            comStub.comStatusOut       -> fprimeFramer.comStatusIn
            fprimeFramer.comStatusOut  -> comQueue.comStatusIn
        }

        connections Uplink {
            # ComDriver buffer allocations
            comDriver.allocate      -> commsBufferManager.bufferGetCallee
            comDriver.deallocate    -> commsBufferManager.bufferSendIn
            # ComDriver <-> ComStub
            comDriver.$recv             -> comStub.drvReceiveIn
            comStub.drvReceiveReturnOut -> comDriver.recvReturnIn
            # ComStub <-> FrameAccumulator
            comStub.dataOut                -> frameAccumulator.dataIn
            frameAccumulator.dataReturnOut -> comStub.dataReturnIn
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


    } # end topology
} # end ComFprime Subtopology
