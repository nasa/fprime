module ComCcsds {

}

module FramingCcsds {

# Usage Note:
#
# When importing this subtopology, users shall establish 5 port connections with a component implementing
# the Svc.Com (Svc/Interfaces/Com.fpp) interface. They are as follows:
#
# 1) Outputs:
#     - FramingCcsds.framer.dataOut                 -> [Svc.Com].dataIn
#     - FramingCcsds.frameAccumulator.dataReturnOut -> [Svc.Com].dataReturnIn
# 2) Inputs:
#     - [Svc.Com].dataReturnOut -> FramingCcsds.framer.dataReturnIn
#     - [Svc.Com].comStatusOut  -> FramingCcsds.framer.comStatusIn
#     - [Svc.Com].dataOut       -> FramingCcsds.frameAccumulator.dataIn


    # ComPacket Queue enum for queue types
    enum Ports_ComPacketQueue {
        EVENTS,
        TELEMETRY 
    }

    enum Ports_ComBufferQueue {
        FILE
    }

    # ----------------------------------------------------------------------
    # Active Components
    # ----------------------------------------------------------------------
    instance comQueue: Svc.ComQueue base id FramingCcsdsConfig.BASE_ID + 0x00000 \
        queue size FramingCcsdsConfig.QueueSizes.comQueue \
        stack size FramingCcsdsConfig.StackSizes.comQueue \
        priority FramingCcsdsConfig.Priorities.comQueue \
    {
        phase Fpp.ToCpp.Phases.configComponents """
        using namespace FramingCcsds;
        using namespace ComCcsds;
        Svc::ComQueue::QueueConfigurationTable configurationTable;

        // Events (highest-priority)
        configurationTable.entries[Ports_ComPacketQueue::EVENTS].depth = FramingCcsdsConfig::QueueDepths::events;
        configurationTable.entries[Ports_ComPacketQueue::EVENTS].priority = FramingCcsdsConfig::QueuePriorities::events;

        // Telemetry
        configurationTable.entries[Ports_ComPacketQueue::TELEMETRY].depth = FramingCcsdsConfig::QueueDepths::tlm;
        configurationTable.entries[Ports_ComPacketQueue::TELEMETRY].priority = FramingCcsdsConfig::QueuePriorities::tlm;

        // File Downlink Queue (buffer queue using NUM_CONSTANTS offset)
        configurationTable.entries[Ports_ComPacketQueue::NUM_CONSTANTS + Ports_ComBufferQueue::FILE].depth = FramingCcsdsConfig::QueueDepths::file;
        configurationTable.entries[Ports_ComPacketQueue::NUM_CONSTANTS + Ports_ComBufferQueue::FILE].priority = FramingCcsdsConfig::QueuePriorities::file;

        // Allocation identifier is 0 as the MallocAllocator discards it
        FramingCcsds::comQueue.configure(configurationTable, 0, FramingCcsds::Allocation::memAllocator);
        """
        phase Fpp.ToCpp.Phases.tearDownComponents """
        FramingCcsds::comQueue.cleanup();
        """
    }

    # ----------------------------------------------------------------------
    # Passive Components
    # ----------------------------------------------------------------------
    instance frameAccumulator: Svc.FrameAccumulator base id FramingCcsdsConfig.BASE_ID + 0x01000 \ 
    {

        phase Fpp.ToCpp.Phases.configObjects """
        Svc::FrameDetectors::CcsdsTcFrameDetector frameDetector;
        """
        phase Fpp.ToCpp.Phases.configComponents """
        FramingCcsds::frameAccumulator.configure(
            ConfigObjects::FramingCcsds_frameAccumulator::frameDetector,
            1,
            FramingCcsds::Allocation::memAllocator,
            FramingCcsdsConfig::BuffMgr::frameAccumulatorSize
        );
        """

        phase Fpp.ToCpp.Phases.tearDownComponents """
        FramingCcsds::frameAccumulator.cleanup();
        """
    }

    instance commsBufferManager: Svc.BufferManager base id FramingCcsdsConfig.BASE_ID + 0x02000 \
    {
        phase Fpp.ToCpp.Phases.configObjects """
        Svc::BufferManager::BufferBins bins;
        """

        phase Fpp.ToCpp.Phases.configComponents """
        memset(&ConfigObjects::FramingCcsds_commsBufferManager::bins, 0, sizeof(ConfigObjects::FramingCcsds_commsBufferManager::bins));
        ConfigObjects::FramingCcsds_commsBufferManager::bins.bins[0].bufferSize = FramingCcsdsConfig::BuffMgr::commsBuffSize;
        ConfigObjects::FramingCcsds_commsBufferManager::bins.bins[0].numBuffers = FramingCcsdsConfig::BuffMgr::commsBuffCount;
        ConfigObjects::FramingCcsds_commsBufferManager::bins.bins[1].bufferSize = FramingCcsdsConfig::BuffMgr::commsFileBuffSize;
        ConfigObjects::FramingCcsds_commsBufferManager::bins.bins[1].numBuffers = FramingCcsdsConfig::BuffMgr::commsFileBuffCount;
        FramingCcsds::commsBufferManager.setup(
            FramingCcsdsConfig::BuffMgr::commsBuffMgrId,
            0,
            FramingCcsds::Allocation::memAllocator,
            ConfigObjects::FramingCcsds_commsBufferManager::bins
        );
        """

        phase Fpp.ToCpp.Phases.tearDownComponents """
        FramingCcsds::commsBufferManager.cleanup();
        """
    }

    instance fprimeRouter: Svc.FprimeRouter base id FramingCcsdsConfig.BASE_ID + 0x03000

    instance tcDeframer: Svc.Ccsds.TcDeframer base id FramingCcsdsConfig.BASE_ID + 0x04000

    instance spacePacketDeframer: Svc.Ccsds.SpacePacketDeframer base id FramingCcsdsConfig.BASE_ID + 0x05000
    # NOTE: name 'framer' is used for the framer that connects to the Com Adapter Interface for better subtopology interoperability
    instance framer: Svc.Ccsds.TmFramer base id FramingCcsdsConfig.BASE_ID + 0x06000

    instance spacePacketFramer: Svc.Ccsds.SpacePacketFramer base id FramingCcsdsConfig.BASE_ID + 0x07000

    instance apidManager: Svc.Ccsds.ApidManager base id FramingCcsdsConfig.BASE_ID + 0x08000

    topology Subtopology {
        # Active Components
        instance comQueue

        # Passive Components
        instance commsBufferManager
        instance frameAccumulator
        instance fprimeRouter
        instance tcDeframer
        instance spacePacketDeframer
        instance framer
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
            spacePacketFramer.dataOut -> framer.dataIn
            framer.dataReturnOut      -> spacePacketFramer.dataReturnIn
            # ComStatus
            framer.comStatusOut            -> spacePacketFramer.comStatusIn
            spacePacketFramer.comStatusOut -> comQueue.comStatusIn
            # (Outgoing) Framer <-> ComInterface connections shall be established by the user
        }

        connections Uplink {
            # (Incoming) ComInterface <-> FrameAccumulator connections shall be established by the user
            # FrameAccumulator buffer allocations
            frameAccumulator.bufferDeallocate -> commsBufferManager.bufferSendIn
            frameAccumulator.bufferAllocate   -> commsBufferManager.bufferGetCallee
            # FrameAccumulator <-> TcDeframer
            frameAccumulator.dataOut -> tcDeframer.dataIn
            tcDeframer.dataReturnOut -> frameAccumulator.dataReturnIn
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
} # end FramingCcsds Subtopology
