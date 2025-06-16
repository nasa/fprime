module CommsCCSDS {

    enum Ports_ComPacketQueue {
        EVENTS,
        TELEMETRY,
        FILE_QUEUE 
    };

    # ----------------------------------------------------------------------
    # Active Components
    # ----------------------------------------------------------------------
    instance comQueue: Svc.ComQueue base id CommsCCSDSConfig.BASE_ID + 0x0100 \
        queue size CommsCCSDSConfig.QueueSizes.comQueue \
        stack size CommsCCSDSConfig.StackSizes.comQueue \
        priority CommsCCSDSConfig.Priorities.comQueue \
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
        configurationTable.entries[ConfigConstants::Comms_comQueue::EVENTS].depth = 100;
        configurationTable.entries[ConfigConstants::Comms_comQueue::EVENTS].priority = 0;
        // Telemetry
        configurationTable.entries[ConfigConstants::Comms_comQueue::TELEMETRY].depth = 500;
        configurationTable.entries[ConfigConstants::Comms_comQueue::TELEMETRY].priority = 2;
        // File Downlink Queue
        configurationTable.entries[ConfigConstants::Comms_comQueue::FILE_QUEUE].depth = 100;
        configurationTable.entries[ConfigConstants::Comms_comQueue::FILE_QUEUE].priority = 1;
        // Allocation identifier is 0 as the MallocAllocator discards it
        Comms::comQueue.configure(configurationTable, 0, Comms::Allocation::mallocator);
        """
    }

    instance cmdSeq: Svc.CmdSequencer base id CommsCCSDSConfig.BASE_ID + 0x0200 \
        queue size CommsCCSDSConfig.QueueSizes.cmdSeq \
        stack size CommsCCSDSConfig.StackSizes.cmdSeq \
        priority CommsCCSDSConfig.Priorities.cmdSeq \
    {
        phase Fpp.ToCpp.Phases.configConstants """
        enum {
          CMD_SEQ_BUFFER_SIZE = 5 * 1024
        };
        """

        phase Fpp.ToCpp.Phases.configComponents """
        Comms::cmdSeq.allocateBuffer(0, Comms::Allocation::mallocator, ConfigConstants::Comms_cmdSeq::CMD_SEQ_BUFFER_SIZE);
        """

        phase Fpp.ToCpp.Phases.tearDownComponents """
        Comms::cmdSeq.deallocateBuffer(Comms::Allocation::mallocator);
        """
    }

    # ----------------------------------------------------------------------
    # Passive Components
    # ----------------------------------------------------------------------
    instance commsBufferManager: Svc.BufferManager base id CommsCCSDSConfig.BASE_ID + 0x0500 \
    {
        phase Fpp.ToCpp.Phases.configConstants """
        enum {
          // Buffer Manager for Uplink/Downlink
          COMMS_BUFFER_MANAGER_STORE_SIZE = 2048,
          COMMS_BUFFER_MANAGER_STORE_COUNT = 20,
          COMMS_BUFFER_MANAGER_FILE_STORE_SIZE = 3000,
          COMMS_BUFFER_MANAGER_FILE_QUEUE_SIZE = 30,
          COMMS_BUFFER_MANAGER_ID = 200
        };
        """

        phase Fpp.ToCpp.Phases.configComponents """
        // Buffer managers need a configured set of buckets and an allocator used to allocate memory for those buckets.
        memset(&Comms::BufferManagerBins::bins, 0, sizeof(Comms::BufferManagerBins::bins));
        Comms::BufferManagerBins::bins.bins[0].bufferSize = ConfigConstants::Comms_commsBufferManager::COMMS_BUFFER_MANAGER_STORE_SIZE;
        Comms::BufferManagerBins::bins.bins[0].numBuffers = ConfigConstants::Comms_commsBufferManager::COMMS_BUFFER_MANAGER_STORE_COUNT;
        Comms::BufferManagerBins::bins.bins[1].bufferSize = ConfigConstants::Comms_commsBufferManager::COMMS_BUFFER_MANAGER_FILE_STORE_SIZE;
        Comms::BufferManagerBins::bins.bins[1].numBuffers = ConfigConstants::Comms_commsBufferManager::COMMS_BUFFER_MANAGER_FILE_QUEUE_SIZE;
        Comms::commsBufferManager.setup(
            ConfigConstants::Comms_commsBufferManager::COMMS_BUFFER_MANAGER_ID,
            0,
            Comms::Allocation::mallocator,
            Comms::BufferManagerBins::bins
        );
        """

        phase Fpp.ToCpp.Phases.tearDownComponents """
        Comms::commsBufferManager.cleanup();
        """
    }

    instance frameAccumulator: Svc.FrameAccumulator base id CommsCCSDSConfig.BASE_ID + 0x0600 \ 
    {

        phase Fpp.ToCpp.Phases.configComponents """
        Comms::frameAccumulator.configure(
            Comms::Detector::frameDetector,
            1,
            Comms::Allocation::mallocator,
            2048
        );
        """

        phase Fpp.ToCpp.Phases.tearDownComponents """
        Comms::frameAccumulator.cleanup();
        """
    }

    instance fprimeRouter: Svc.FprimeRouter base id CommsCCSDSConfig.BASE_ID + 0x0700 \
    
    instance comStub: Svc.ComStub base id CommsCCSDSConfig.BASE_ID + 0x0800 \

    instance tcDeframer: Svc.CCSDS.TcDeframer base id CommsCCSDSConfig.BASE_ID + 0x0900 \

    instance spacePacketDeframer: Svc.CCSDS.SpacePacketDeframer base id CommsCCSDSConfig.BASE_ID + 0x0A00 \

    instance tmFramer: Svc.CCSDS.TmFramer base id CommsCCSDSConfig.BASE_ID + 0x0B00 \

    instance spacePacketFramer: Svc.CCSDS.SpacePacketFramer base id CommsCCSDSConfig.BASE_ID + 0x0C00 \

    instance apidManager: Svc.CCSDS.ApidManager base id CommsCCSDSConfig.BASE_ID + 0x0D00 \

    topology Subtopology {
        # Active Components
        instance comQueue
        instance cmdSeq

        # Passive Components
        instance commsBufferManager
        instance frameAccumulator
        instance fprimeRouter
        instance comStub
        instance comDriver
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
            # ComStub <-> ComDriver
            comStub.drvSendOut      -> comDriver.$send
            comDriver.sendReturnOut -> comStub.drvSendReturnIn
            comDriver.ready         -> comStub.drvConnected
            # ComStatus
            comStub.comStatusOut            -> tmFramer.comStatusIn
            tmFramer.comStatusOut           -> spacePacketFramer.comStatusIn
            spacePacketFramer.comStatusOut  -> comQueue.comStatusIn
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
            # Router <-> CmdDispatcher/FileUplink
            fprimeRouter.commandOut  -> cmdDisp.seqCmdBuff
            cmdDisp.seqCmdStatus     -> fprimeRouter.cmdResponseIn
            fprimeRouter.fileOut     -> fileUplink.bufferSendIn
            fileUplink.bufferSendOut -> fprimeRouter.fileBufferReturnIn
        }

    } # end topology
} # end Comms Subtopology