module ComCcsds {

    enum Ports_ComPacketQueue {
        EVENTS,
        TELEMETRY,
        FILE_QUEUE 
    };

    # ----------------------------------------------------------------------
    # Active Components
    # ----------------------------------------------------------------------
    instance comQueue: Svc.ComQueue base id ComCcsdsConfig.BASE_ID + 0x0100 \
        queue size ComCcsdsConfig.QueueSizes.comQueue \
        stack size ComCcsdsConfig.StackSizes.comQueue \
        priority ComCcsdsConfig.Priorities.comQueue \
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
        configurationTable.entries[ConfigConstants::ComCcsds_comQueue::EVENTS].depth = 100;
        configurationTable.entries[ConfigConstants::ComCcsds_comQueue::EVENTS].priority = 0;
        // Telemetry
        configurationTable.entries[ConfigConstants::ComCcsds_comQueue::TELEMETRY].depth = 500;
        configurationTable.entries[ConfigConstants::ComCcsds_comQueue::TELEMETRY].priority = 2;
        // File Downlink Queue
        configurationTable.entries[ConfigConstants::ComCcsds_comQueue::FILE_QUEUE].depth = 100;
        configurationTable.entries[ConfigConstants::ComCcsds_comQueue::FILE_QUEUE].priority = 1;
        // Allocation identifier is 0 as the MallocAllocator discards it
        ComCcsds::comQueue.configure(configurationTable, 0, ComCcsds::Allocation::mallocator);
        """
        phase Fpp.ToCpp.Phases.tearDownComponents """
        ComCcsds::comQueue.cleanup();
        """
    }

    instance cmdSeq: Svc.CmdSequencer base id ComCcsdsConfig.BASE_ID + 0x0200 \
        queue size ComCcsdsConfig.QueueSizes.cmdSeq \
        stack size ComCcsdsConfig.StackSizes.cmdSeq \
        priority ComCcsdsConfig.Priorities.cmdSeq \
    {
        phase Fpp.ToCpp.Phases.configConstants """
        enum {
          CMD_SEQ_BUFFER_SIZE = 5 * 1024
        };
        """

        phase Fpp.ToCpp.Phases.configComponents """
        ComCcsds::cmdSeq.allocateBuffer(0, ComCcsds::Allocation::mallocator, ConfigConstants::ComCcsds_cmdSeq::CMD_SEQ_BUFFER_SIZE);
        """

        phase Fpp.ToCpp.Phases.tearDownComponents """
        ComCcsds::cmdSeq.deallocateBuffer(ComCcsds::Allocation::mallocator);
        """
    }

    # ----------------------------------------------------------------------
    # Passive Components
    # ----------------------------------------------------------------------
    instance frameAccumulator: Svc.FrameAccumulator base id ComCcsdsConfig.BASE_ID + 0x0500 \ 
    {

        phase Fpp.ToCpp.Phases.configComponents """
        ComCcsds::frameAccumulator.configure(
            ComCcsds::Detector::frameDetector,
            1,
            ComCcsds::Allocation::mallocator,
            2048
        );
        """

        phase Fpp.ToCpp.Phases.tearDownComponents """
        ComCcsds::frameAccumulator.cleanup();
        """
    }

    instance commsBufferManager: Svc.BufferManager base id ComCcsdsConfig.BASE_ID + 0x0600 \
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
        memset(&ComCcsds::BufferManagerBins::bins, 0, sizeof(ComCcsds::BufferManagerBins::bins));
        ComCcsds::BufferManagerBins::bins.bins[0].bufferSize = ConfigConstants::ComCcsds_commsBufferManager::COMMS_BUFFER_MANAGER_STORE_SIZE;
        ComCcsds::BufferManagerBins::bins.bins[0].numBuffers = ConfigConstants::ComCcsds_commsBufferManager::COMMS_BUFFER_MANAGER_STORE_COUNT;
        ComCcsds::BufferManagerBins::bins.bins[1].bufferSize = ConfigConstants::ComCcsds_commsBufferManager::COMMS_BUFFER_MANAGER_FILE_STORE_SIZE;
        ComCcsds::BufferManagerBins::bins.bins[1].numBuffers = ConfigConstants::ComCcsds_commsBufferManager::COMMS_BUFFER_MANAGER_FILE_QUEUE_SIZE;
        ComCcsds::commsBufferManager.setup(
            ConfigConstants::ComCcsds_commsBufferManager::COMMS_BUFFER_MANAGER_ID,
            0,
            ComCcsds::Allocation::mallocator,
            ComCcsds::BufferManagerBins::bins
        );
        """

        phase Fpp.ToCpp.Phases.tearDownComponents """
        ComCcsds::commsBufferManager.cleanup();
        """
    }

    instance fprimeRouter: Svc.FprimeRouter base id ComCcsdsConfig.BASE_ID + 0x0700 \
    
    instance comStub: Svc.ComStub base id ComCcsdsConfig.BASE_ID + 0x0800 \

    instance tcDeframer: Svc.CCSDS.TcDeframer base id ComCcsdsConfig.BASE_ID + 0x0900 \

    instance spacePacketDeframer: Svc.CCSDS.SpacePacketDeframer base id ComCcsdsConfig.BASE_ID + 0x0A00 \

    instance tmFramer: Svc.CCSDS.TmFramer base id ComCcsdsConfig.BASE_ID + 0x0B00 \

    instance spacePacketFramer: Svc.CCSDS.SpacePacketFramer base id ComCcsdsConfig.BASE_ID + 0x0C00 \

    instance apidManager: Svc.CCSDS.ApidManager base id ComCcsdsConfig.BASE_ID + 0x0D00 \

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
         
        }

    } # end topology
} # end ComCcsds Subtopology
