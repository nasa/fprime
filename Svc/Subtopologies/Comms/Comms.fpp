module Comms {

    enum Ports_ComPacketQueue {
        EVENTS,
        TELEMETRY,
        FILE_QUEUE 
    };

    # ----------------------------------------------------------------------
    # Active Components
    # ----------------------------------------------------------------------
    instance comQueue: Svc.ComQueue base id CommsConfig.BASE_ID + 0x0100 \
        queue size CommsConfig.QueueSizes.comQueue \
        stack size CommsConfig.StackSizes.comQueue \
        priority CommsConfig.Priorities.comQueue \
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

    instance cmdSeq: Svc.CmdSequencer base id CommsConfig.BASE_ID + 0x0200 \
        queue size CommsConfig.QueueSizes.cmdSeq \
        stack size CommsConfig.StackSizes.cmdSeq \
        priority CommsConfig.Priorities.cmdSeq \
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
    instance commsBufferManager: Svc.BufferManager base id CommsConfig.BASE_ID + 0x0500 \
    {
        phase Fpp.ToCpp.Phases.configConstants """
        enum {
          COMMS_BUFFER_MANAGER_ID = 200,
          COMMS_BUFFER_MANAGER_STORE_SIZE = 2048,
          COMMS_BUFFER_MANAGER_STORE_COUNT = 20,
          COMMS_BUFFER_MANAGER_FILE_STORE_SIZE = 3000,
          COMMS_BUFFER_MANAGER_FILE_QUEUE_SIZE = 30
        };
        """

        phase Fpp.ToCpp.Phases.configComponents """
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

    instance frameAccumulator: Svc.FrameAccumulator base id CommsConfig.BASE_ID + 0x0600 \ 
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

    instance deframer: Svc.FprimeDeframer base id CommsConfig.BASE_ID + 0x0700 \

    instance fprimeFramer: Svc.FprimeFramer base id CommsConfig.BASE_ID + 0x0800 \

    instance fprimeRouter: Svc.FprimeRouter base id CommsConfig.BASE_ID + 0x0900 \
    
    instance comStub: Svc.ComStub base id CommsConfig.BASE_ID + 0x0A00 \

    @ Communications driver. May be swapped with other comm drivers like UART
    instance comDriver: Drv.TcpClient base id CommsConfig.BASE_ID + 0x0B00 \ 
    {
        phase Fpp.ToCpp.Phases.configComponents """
        if (state.hostname != nullptr && state.port != 0) {
            Comms::comDriver.configure(state.hostname, state.port);
        }
        """

        phase Fpp.ToCpp.Phases.startTasks """
        if (state.hostname != nullptr && state.port != 0) {
            Os::TaskString name("ReceiveTask");
            Comms::comDriver.start(name, 100, 100);
        }
        """

        phase Fpp.ToCpp.Phases.stopTasks """
        Comms::comDriver.stop();
        """

        phase Fpp.ToCpp.Phases.freeThreads """
        (void)Comms::comDriver.join();
        """
    }

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
} # end Comms Subtopology