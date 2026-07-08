module SpacePacket {

    # ComPacket Queue enum for queue types
    enum Ports_ComPacketQueue : U8 {
        EVENTS,
        TELEMETRY
    }

    enum Ports_ComBufferQueue : U8 {
        FILE
    }

    # ----------------------------------------------------------------------
    # Active Components
    # ----------------------------------------------------------------------
    instance comQueue: Svc.ComQueue base id SpacePacketConfig.BASE_ID + 0x00000 \
        queue size SpacePacketConfig.QueueSizes.comQueue \
        stack size SpacePacketConfig.StackSizes.comQueue \
        priority SpacePacketConfig.Priorities.comQueue \
    {
        phase Fpp.ToCpp.Phases.configComponents """
        using namespace SpacePacket;
        Svc::ComQueue::QueueConfigurationTable configurationTable;

        // Events (highest-priority)
        configurationTable.entries[Ports_ComPacketQueue::EVENTS].depth = SpacePacketConfig::QueueDepths::events;
        configurationTable.entries[Ports_ComPacketQueue::EVENTS].priority = SpacePacketConfig::QueuePriorities::events;

        // Telemetry
        configurationTable.entries[Ports_ComPacketQueue::TELEMETRY].depth = SpacePacketConfig::QueueDepths::tlm;
        configurationTable.entries[Ports_ComPacketQueue::TELEMETRY].priority = SpacePacketConfig::QueuePriorities::tlm;

        // File Downlink Queue (buffer queue using NUM_CONSTANTS offset)
        configurationTable.entries[Ports_ComPacketQueue::NUM_CONSTANTS + Ports_ComBufferQueue::FILE].depth = SpacePacketConfig::QueueDepths::file;
        configurationTable.entries[Ports_ComPacketQueue::NUM_CONSTANTS + Ports_ComBufferQueue::FILE].priority = SpacePacketConfig::QueuePriorities::file;

        // Allocation identifier is 0 as the MallocAllocator discards it
        SpacePacket::comQueue.configure(configurationTable, 0, SpacePacket::Allocation::memAllocator);
        """
        phase Fpp.ToCpp.Phases.tearDownComponents """
        SpacePacket::comQueue.cleanup();
        """
    }

    # ----------------------------------------------------------------------
    # Passive Components
    # ----------------------------------------------------------------------
    instance commsBufferManager: Svc.BufferManager base id SpacePacketConfig.BASE_ID + 0x01000 \
    {
        phase Fpp.ToCpp.Phases.configObjects """
        Svc::BufferManager::BufferBins bins;
        """

        phase Fpp.ToCpp.Phases.configComponents """
        memset(&ConfigObjects::SpacePacket_commsBufferManager::bins, 0, sizeof(ConfigObjects::SpacePacket_commsBufferManager::bins));
        ConfigObjects::SpacePacket_commsBufferManager::bins.bins[0].bufferSize = SpacePacketConfig::BuffMgr::commsBuffSize;
        ConfigObjects::SpacePacket_commsBufferManager::bins.bins[0].numBuffers = SpacePacketConfig::BuffMgr::commsBuffCount;
        ConfigObjects::SpacePacket_commsBufferManager::bins.bins[1].bufferSize = SpacePacketConfig::BuffMgr::commsFileBuffSize;
        ConfigObjects::SpacePacket_commsBufferManager::bins.bins[1].numBuffers = SpacePacketConfig::BuffMgr::commsFileBuffCount;
        SpacePacket::commsBufferManager.setup(
            SpacePacketConfig::BuffMgr::commsBuffMgrId,
            0,
            SpacePacket::Allocation::memAllocator,
            ConfigObjects::SpacePacket_commsBufferManager::bins
        );
        """

        phase Fpp.ToCpp.Phases.tearDownComponents """
        SpacePacket::commsBufferManager.cleanup();
        """
    }

    instance fprimeRouter: Svc.FprimeRouter base id SpacePacketConfig.BASE_ID + 0x02000

    instance spacePacketDeframer: Svc.Ccsds.SpacePacketDeframer base id SpacePacketConfig.BASE_ID + 0x03000

    instance aggregator: Svc.ComAggregator base id SpacePacketConfig.BASE_ID + 0x04000 \
        queue size SpacePacketConfig.QueueSizes.aggregator \
        stack size SpacePacketConfig.StackSizes.aggregator \
        priority SpacePacketConfig.Priorities.aggregator

    instance spacePacketFramer: Svc.Ccsds.SpacePacketFramer base id SpacePacketConfig.BASE_ID + 0x05000

    instance apidManager: Svc.Ccsds.ApidManager base id SpacePacketConfig.BASE_ID + 0x06000

    instance comStub: Svc.ComStub base id SpacePacketConfig.BASE_ID + 0x07000

    topology FramingSubtopology {
        # Usage Note:
        #
        # When importing this subtopology, users shall establish 5 port connections with a downstream
        # framing layer or a component implementing the Svc.Com (Svc/Interfaces/Com.fpp) interface:
        #
        # 1) Outputs:
        #     - SpacePacket.aggregator.dataOut                 -> [downstream].dataIn
        #     - SpacePacket.spacePacketDeframer.dataReturnOut  -> [downstream].dataReturnIn
        # 2) Inputs:
        #     - [downstream].dataReturnOut -> SpacePacket.aggregator.dataReturnIn
        #     - [downstream].comStatusOut  -> SpacePacket.aggregator.comStatusIn
        #     - [downstream].dataOut       -> SpacePacket.spacePacketDeframer.dataIn

        # Active Components
        instance comQueue

        # Passive Components
        instance commsBufferManager
        instance fprimeRouter
        instance spacePacketDeframer
        instance spacePacketFramer
        instance apidManager
        instance aggregator

        connections Downlink {
            # ComQueue <-> SpacePacketFramer
            comQueue.dataOut                -> spacePacketFramer.dataIn
            spacePacketFramer.dataReturnOut -> comQueue.dataReturnIn
            # SpacePacketFramer buffer and APID management
            spacePacketFramer.bufferAllocate   -> commsBufferManager.bufferGetCallee
            spacePacketFramer.bufferDeallocate -> commsBufferManager.bufferSendIn
            spacePacketFramer.getApidSeqCount  -> apidManager.getApidSeqCountIn
            # SpacePacketFramer <-> ComAggregator
            spacePacketFramer.dataOut -> aggregator.dataIn
            aggregator.dataReturnOut  -> spacePacketFramer.dataReturnIn

            # ComStatus
            aggregator.comStatusOut        -> spacePacketFramer.comStatusIn
            spacePacketFramer.comStatusOut -> comQueue.comStatusIn
            # (Outgoing) Aggregator <-> downstream connections shall be established by the user
        }

        connections Uplink {
            # (Incoming) downstream <-> SpacePacketDeframer connections shall be established by the user
            # SpacePacketDeframer APID validation
            spacePacketDeframer.validateApidSeqCount -> apidManager.validateApidSeqCountIn
            # SpacePacketDeframer <-> Router
            spacePacketDeframer.dataOut -> fprimeRouter.dataIn
            fprimeRouter.dataReturnOut  -> spacePacketDeframer.dataReturnIn
        }
    } # end FramingSubtopology

    # This subtopology uses FramingSubtopology with a ComStub component for Com Interface
    topology Subtopology {
        import FramingSubtopology

        instance comStub

        connections ComStub {
            # Aggregator <-> ComStub (Downlink)
            SpacePacket.aggregator.dataOut -> comStub.dataIn
            comStub.dataReturnOut          -> SpacePacket.aggregator.dataReturnIn
            comStub.comStatusOut           -> SpacePacket.aggregator.comStatusIn

            # ComStub <-> SpacePacketDeframer (Uplink)
            comStub.dataOut -> SpacePacket.spacePacketDeframer.dataIn
            SpacePacket.spacePacketDeframer.dataReturnOut -> comStub.dataReturnIn
        }

        # ----------------------------------------------------------------------
        # Topology ports
        # ----------------------------------------------------------------------

        # Command routing
        @ Output port sending routed command packets to the command dispatcher
        port commandOut         = fprimeRouter.commandOut

        @ Input port receiving command response messages back into the router
        port cmdResponseIn      = fprimeRouter.cmdResponseIn

        @ Output port sending uplinked file packets to the file handling stack
        port fileUplinkOut          = fprimeRouter.fileOut

        @ Input port receiving back buffer ownership from the file handling stack
        port fileUplinkReturnIn = fprimeRouter.fileBufferReturnIn

        # Telemetry/events/file queuing (array ports - index at connection site)
        @ Input port array for queueing Fw::ComBuffers
        port comPacketQueueIn = comQueue.comPacketQueueIn

        @ Input port array for queueing Fw::Buffers
        port bufferQueueIn    = comQueue.bufferQueueIn

        @ Output port array returning ownership of Fw::Buffers to their original sender after dequeuing
        port bufferReturnOut  = comQueue.bufferReturnOut

        # ComDriver interface (via ComStub)
        @ Input port receiving data read from the ByteStream driver
        port drvReceiveIn        = comStub.drvReceiveIn

        @ Output port returning ownership of the buffer that came in on drvReceiveIn back to the driver
        port drvReceiveReturnOut = comStub.drvReceiveReturnOut

        @ Output port sending framed data to the ByteStream driver for transmission
        port drvSendOut          = comStub.drvSendOut

        @ Input port receiving the ready signal when the ByteStream driver has connected
        port drvConnected        = comStub.drvConnected

        # Buffer management for ComDriver
        @ Input port for requesting (allocating) a new Fw::Buffer from the comms buffer pool
        port commsBufferGetCallee = commsBufferManager.bufferGetCallee

        @ Input port for deallocating Fw::Buffers back into the comms buffer pool
        port commsBufferSendIn    = commsBufferManager.bufferSendIn

        # Scheduling
        @ Input port for scheduling ComQueue telemetry output
        port comQueueRun          = comQueue.run

        @ Rate-group driven timeout to flush the ComAggregator buffer
        port aggregatorTimeout    = aggregator.timeout

        @ Input port triggering commsBufferManager telemetry output
        port bufferManagerSchedIn = commsBufferManager.schedIn

    } # end Subtopology

} # end SpacePacket
