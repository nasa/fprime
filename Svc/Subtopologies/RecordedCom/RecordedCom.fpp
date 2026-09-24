module RecordedCom {
    # ----------------------------------------------------------------------
    # Active Components
    # ----------------------------------------------------------------------
    instance comLogger: Svc.ComLoggerDp base id RecordedComConfig.BASE_ID + 0x0000 \
        queue size RecordedComConfig.QueueSizes.comLogger \
        stack size RecordedComConfig.StackSizes.comLogger \
        priority RecordedComConfig.Priorities.comLogger \
        cpu RecordedComConfig.CpuAffinities.comLogger \
    {
        phase Fpp.ToCpp.Phases.configComponents """
        RecordedCom::comLogger.configure(
            RecordedComConfig::ComLogger::enabled,
            RecordedComConfig::ComLogger::packetsPerContainer,
            RecordedComConfig::ComLogger::defaultPriority,
            RecordedComConfig::ComLogger::flushTimeout
        );
        """
    }

    instance dpMgr: Svc.DpManager base id RecordedComConfig.BASE_ID + 0x1000 \
        queue size RecordedComConfig.QueueSizes.dpMgr \
        stack size RecordedComConfig.StackSizes.dpMgr \
        priority RecordedComConfig.Priorities.dpMgr \
        cpu RecordedComConfig.CpuAffinities.dpMgr

    instance dpWriter: Svc.DpWriter base id RecordedComConfig.BASE_ID + 0x2000 \
        queue size RecordedComConfig.QueueSizes.dpWriter \
        stack size RecordedComConfig.StackSizes.dpWriter \
        priority RecordedComConfig.Priorities.dpWriter \
        cpu RecordedComConfig.CpuAffinities.dpWriter \
    {
        phase Fpp.ToCpp.Phases.configComponents """
        Fw::FileNameString recordedComDpDir(RecordedComConfig::Paths::dpDir);
        Os::FileSystem::createDirectory(recordedComDpDir.toChar());
        RecordedCom::dpWriter.configure(recordedComDpDir);
        """
    }

    # ----------------------------------------------------------------------
    # Passive Components
    # ----------------------------------------------------------------------
    instance dpBufferManager: Svc.BufferManager base id RecordedComConfig.BASE_ID + 0x3000 \
    {
        phase Fpp.ToCpp.Phases.configObjects """
        Svc::BufferManager::BufferBins bins;
        """
        phase Fpp.ToCpp.Phases.configComponents """
        memset(&ConfigObjects::RecordedCom_dpBufferManager::bins, 0, sizeof(ConfigObjects::RecordedCom_dpBufferManager::bins));
        ConfigObjects::RecordedCom_dpBufferManager::bins.bins[0].bufferSize = RecordedComConfig::BuffMgr::dpBufferStoreSize;
        ConfigObjects::RecordedCom_dpBufferManager::bins.bins[0].numBuffers = RecordedComConfig::BuffMgr::dpBufferStoreCount;
        RecordedCom::dpBufferManager.setup(
            RecordedComConfig::BuffMgr::dpBufferManagerId,
            0,
            RecordedCom::Allocation::memAllocator,
            ConfigObjects::RecordedCom_dpBufferManager::bins
        );
        """
        phase Fpp.ToCpp.Phases.tearDownComponents """
        RecordedCom::dpBufferManager.cleanup();
        """
    }

    @ A subtopology with ComLoggerDp and supporting data product infrastructure
    topology Subtopology {
        # ----------------------------------------------------------------------
        # Component instances
        # ----------------------------------------------------------------------
        instance comLogger
        instance dpMgr
        instance dpWriter
        instance dpBufferManager

        # ----------------------------------------------------------------------
        # Internal connections
        # ----------------------------------------------------------------------
        connections RecordedCom {
            # ComLoggerDp to DpManager
            comLogger.productGetOut[0] -> dpMgr.productGetIn[0]
            comLogger.productSendOut[0] -> dpMgr.productSendIn[0]

            # DpManager to BufferManager
            dpMgr.bufferGetOut[0] -> dpBufferManager.bufferGetCallee

            # DpManager to DpWriter
            dpMgr.productSendOut[0] -> dpWriter.bufferSendIn

            # DpWriter to BufferManager (deallocation)
            dpWriter.deallocBufferSendOut -> dpBufferManager.bufferSendIn
        }

        # ----------------------------------------------------------------------
        # External ports
        # ----------------------------------------------------------------------

        @ Input port for COM buffers to record
        port comIn = comLogger.comIn

        @ Input port to start recording via port call
        port startRecordingIn = comLogger.startRecordingIn

        @ Input port to stop recording via port call
        port stopRecordingIn = comLogger.stopRecordingIn

        @ Input port for health ping
        port pingIn = comLogger.pingIn

        @ Output port for ping response
        port pingOut = comLogger.pingOut

        @ Input port for scheduling ComLoggerDp telemetry
        port comLoggerSchedIn = comLogger.schedIn

        @ Input port for scheduling DpManager telemetry
        port dpMgrSchedIn = dpMgr.schedIn

        @ Input port for scheduling DpWriter telemetry
        port dpWriterSchedIn = dpWriter.schedIn

        @ Input port for scheduling BufferManager telemetry
        port dpBufferManagerSchedIn = dpBufferManager.schedIn

    } # end topology
} # end RecordedCom