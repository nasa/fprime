module ComCcsds {

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
    instance comQueue: Svc.ComQueue base id ComCcsdsConfig.BASE_ID + 0x00000 \
        queue size ComCcsdsConfig.QueueSizes.comQueue \
        stack size ComCcsdsConfig.StackSizes.comQueue \
        priority ComCcsdsConfig.Priorities.comQueue \
    {
        phase Fpp.ToCpp.Phases.configComponents """
        using namespace ComCcsds;
        Svc::ComQueue::QueueConfigurationTable configurationTable;

        // Events (highest-priority)
        configurationTable.entries[Ports_ComPacketQueue::EVENTS].depth = ComCcsdsConfig::QueueDepths::events;
        configurationTable.entries[Ports_ComPacketQueue::EVENTS].priority = ComCcsdsConfig::QueuePriorities::events;

        // Telemetry
        configurationTable.entries[Ports_ComPacketQueue::TELEMETRY].depth = ComCcsdsConfig::QueueDepths::tlm;
        configurationTable.entries[Ports_ComPacketQueue::TELEMETRY].priority = ComCcsdsConfig::QueuePriorities::tlm;

        // File Downlink Queue (buffer queue using NUM_CONSTANTS offset)
        configurationTable.entries[Ports_ComPacketQueue::NUM_CONSTANTS + Ports_ComBufferQueue::FILE].depth = ComCcsdsConfig::QueueDepths::file;
        configurationTable.entries[Ports_ComPacketQueue::NUM_CONSTANTS + Ports_ComBufferQueue::FILE].priority = ComCcsdsConfig::QueuePriorities::file;

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
    instance frameAccumulator: Svc.FrameAccumulator base id ComCcsdsConfig.BASE_ID + 0x01000 \ 
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

    instance commsBufferManager: Svc.BufferManager base id ComCcsdsConfig.BASE_ID + 0x02000 \
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

    instance fprimeRouter: Svc.FprimeRouter base id ComCcsdsConfig.BASE_ID + 0x03000

    instance tcDeframer: Svc.Ccsds.TcDeframer base id ComCcsdsConfig.BASE_ID + 0x04000

    instance spacePacketDeframer: Svc.Ccsds.SpacePacketDeframer base id ComCcsdsConfig.BASE_ID + 0x05000

    instance aggregator: Svc.ComAggregator base id ComCcsdsConfig.BASE_ID + 0x06000 \
        queue size ComCcsdsConfig.QueueSizes.aggregator \
        stack size ComCcsdsConfig.StackSizes.aggregator \
        priority ComCcsdsConfig.Priorities.aggregator

    # NOTE: name 'framer' is used for the framer that connects to the Com Adapter Interface for better subtopology interoperability
    instance framer: Svc.Ccsds.TmFramer base id ComCcsdsConfig.BASE_ID + 0x07000

    instance spacePacketFramer: Svc.Ccsds.SpacePacketFramer base id ComCcsdsConfig.BASE_ID + 0x08000

    instance apidManager: Svc.Ccsds.ApidManager base id ComCcsdsConfig.BASE_ID + 0x09000

    instance comStub: Svc.ComStub base id ComCcsdsConfig.BASE_ID + 0x0A000

    # This subtopology boxes the Space Packet packet layer: router, ComQueue, space packet
    # framer/deframer, APID manager, aggregator, and comms buffer manager.
    topology SpacePacketFraming {
        # Usage Note:
        #
        # When importing this subtopology, users shall establish 5 port connections with a downstream
        # framing layer or a component implementing the Svc.Com (Svc/Interfaces/Com.fpp) interface:
        #
        # 1) Outputs:
        #     - ComCcsds.SpacePacketFraming.dataOut       -> [downstream].dataIn
        #     - ComCcsds.SpacePacketFraming.dataReturnOut -> [downstream].dataReturnIn
        # 2) Inputs:
        #     - [downstream].dataReturnOut -> ComCcsds.SpacePacketFraming.dataReturnIn
        #     - [downstream].comStatusOut  -> ComCcsds.SpacePacketFraming.comStatusIn
        #     - [downstream].dataOut       -> ComCcsds.SpacePacketFraming.dataIn

        include "SpacePacketFraming.fppi"

        # ----------------------------------------------------------------------
        # Topology ports (open framing boundary)
        # ----------------------------------------------------------------------

        @ Output port sending aggregated space packets to the downstream framing layer
        port dataOut       = aggregator.dataOut

        @ Input port receiving back ownership of downlinked buffers from the downstream framing layer
        port dataReturnIn  = aggregator.dataReturnIn

        @ Input port receiving com status from the downstream framing layer
        port comStatusIn   = aggregator.comStatusIn

        @ Input port receiving space packets from the downstream framing layer for deframing
        port dataIn        = spacePacketDeframer.dataIn

        @ Output port returning ownership of uplinked buffers to the downstream framing layer
        port dataReturnOut = spacePacketDeframer.dataReturnOut

        # Buffer management boundary
        @ Input port for requesting (allocating) a new Fw::Buffer from the comms buffer pool
        port bufferGetCallee = commsBufferManager.bufferGetCallee

        @ Input port for deallocating Fw::Buffers back into the comms buffer pool
        port bufferSendIn    = commsBufferManager.bufferSendIn
    } # end SpacePacketFraming

    # This subtopology uses SpacePacketFraming with a ComStub component for Com Interface,
    # providing a space-packet-only stack with no transfer frame layer.
    topology SpacePacket {
        include "SpacePacketFraming.fppi"

        instance comStub

        connections SpacePacketComStub {
            # SpacePacketFraming <-> ComStub (Downlink)
            aggregator.dataOut    -> comStub.dataIn
            comStub.dataReturnOut -> aggregator.dataReturnIn
            comStub.comStatusOut  -> aggregator.comStatusIn

            # ComStub <-> SpacePacketFraming (Uplink)
            comStub.dataOut -> spacePacketDeframer.dataIn
            spacePacketDeframer.dataReturnOut -> comStub.dataReturnIn
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

    } # end SpacePacket

    # This subtopology boxes the CCSDS TM/TC transfer frame layer: the TM framer (downlink),
    # and the frame accumulator + TC deframer (uplink).
    topology TmTcFraming {
        # Usage Note:
        #
        # When importing this subtopology, users shall establish the following external connections:
        #
        # 1) Upstream (packet layer, e.g. SpacePacketFraming):
        #     - [upstream].dataOut                          -> ComCcsds.TmTcFraming.dataIn
        #     - ComCcsds.TmTcFraming.dataReturnOut          -> [upstream].dataReturnIn
        #     - ComCcsds.TmTcFraming.comStatusOut           -> [upstream].comStatusIn
        #     - ComCcsds.TmTcFraming.dataOut                -> [upstream].dataIn (deframed data)
        #     - [upstream].dataReturnOut                    -> ComCcsds.TmTcFraming.dataReturnIn
        # 2) Downstream (a component implementing the Svc.Com interface):
        #     - ComCcsds.TmTcFraming.framedDataOut          -> [Svc.Com].dataIn
        #     - ComCcsds.TmTcFraming.framedDataReturnOut    -> [Svc.Com].dataReturnIn
        #     - [Svc.Com].dataReturnOut -> ComCcsds.TmTcFraming.framedDataReturnIn
        #     - [Svc.Com].comStatusOut  -> ComCcsds.TmTcFraming.framedComStatusIn
        #     - [Svc.Com].dataOut       -> ComCcsds.TmTcFraming.framedDataIn
        # 3) Buffer management (e.g. a Svc.BufferManager):
        #     - ComCcsds.TmTcFraming.bufferAllocate   -> [BufferManager].bufferGetCallee
        #     - ComCcsds.TmTcFraming.bufferDeallocate -> [BufferManager].bufferSendIn

        include "TmTcFraming.fppi"

        # ----------------------------------------------------------------------
        # Topology ports
        # ----------------------------------------------------------------------

        # Upstream boundary (packet layer)
        @ Input port receiving space packets from the packet layer for TM framing
        port dataIn        = framer.dataIn

        @ Output port returning ownership of downlinked buffers to the packet layer
        port dataReturnOut = framer.dataReturnOut

        @ Output port forwarding com status to the packet layer
        port comStatusOut  = framer.comStatusOut

        @ Output port sending TC-deframed data to the packet layer
        port dataOut       = tcDeframer.dataOut

        @ Input port receiving back ownership of uplinked buffers from the packet layer
        port dataReturnIn  = tcDeframer.dataReturnIn

        # Downstream boundary (Svc.Com interface)
        @ Output port sending TM transfer frames to the com interface
        port framedDataOut       = framer.dataOut

        @ Input port receiving back ownership of transmitted frame buffers from the com interface
        port framedDataReturnIn  = framer.dataReturnIn

        @ Input port receiving com status from the com interface
        port framedComStatusIn   = framer.comStatusIn

        @ Input port receiving raw uplink data from the com interface
        port framedDataIn        = frameAccumulator.dataIn

        @ Output port returning ownership of received uplink buffers to the com interface
        port framedDataReturnOut = frameAccumulator.dataReturnOut

        # Buffer management boundary
        @ Output port for allocating accumulation buffers
        port bufferAllocate   = frameAccumulator.bufferAllocate

        @ Output port for deallocating accumulation buffers
        port bufferDeallocate = frameAccumulator.bufferDeallocate
    } # end TmTcFraming

    # This subtopology composes the SpacePacketFraming packet layer with the TmTcFraming
    # TM/TC transfer frame layer to form the full CCSDS communications stack.
    topology FramingSubtopology {
        # Usage Note:
        #
        # When importing this subtopology, users shall establish 5 port connections with a component implementing
        # the Svc.Com (Svc/Interfaces/Com.fpp) interface. They are as follows:
        #
        # 1) Outputs:
        #     - ComCcsds.FramingSubtopology.dataOut       -> [Svc.Com].dataIn
        #     - ComCcsds.FramingSubtopology.dataReturnOut -> [Svc.Com].dataReturnIn
        # 2) Inputs:
        #     - [Svc.Com].dataReturnOut -> ComCcsds.FramingSubtopology.dataReturnIn
        #     - [Svc.Com].comStatusOut  -> ComCcsds.FramingSubtopology.comStatusIn
        #     - [Svc.Com].dataOut       -> ComCcsds.FramingSubtopology.dataIn

        # Packet layer (router, ComQueue, space packet framer/deframer, buffer manager)
        include "SpacePacketFraming.fppi"

        # TM/TC transfer frame layer (TM framer, frame accumulator, TC deframer)
        include "TmTcFraming.fppi"

        # Connections composing the packet layer with the transfer frame layer
        include "FramingInterconnect.fppi"

        # ----------------------------------------------------------------------
        # Topology ports (Svc.Com boundary)
        # ----------------------------------------------------------------------

        @ Output port sending TM transfer frames to the com interface
        port dataOut       = framer.dataOut

        @ Input port receiving back ownership of transmitted frame buffers from the com interface
        port dataReturnIn  = framer.dataReturnIn

        @ Input port receiving com status from the com interface
        port comStatusIn   = framer.comStatusIn

        @ Input port receiving raw uplink data from the com interface
        port dataIn        = frameAccumulator.dataIn

        @ Output port returning ownership of received uplink buffers to the com interface
        port dataReturnOut = frameAccumulator.dataReturnOut
    } # end FramingSubtopology

    # This subtopology uses FramingSubtopology with a ComStub component for Com Interface
    topology Subtopology {
        # Packet layer (router, ComQueue, space packet framer/deframer, buffer manager)
        include "SpacePacketFraming.fppi"

        # TM/TC transfer frame layer (TM framer, frame accumulator, TC deframer)
        include "TmTcFraming.fppi"

        # Connections composing the packet layer with the transfer frame layer
        include "FramingInterconnect.fppi"

        instance comStub

        connections ComStub {
            # TmTcFraming <-> ComStub (Downlink)
            framer.dataOut        -> comStub.dataIn
            comStub.dataReturnOut -> framer.dataReturnIn
            comStub.comStatusOut  -> framer.comStatusIn

            # ComStub <-> TmTcFraming (Uplink)
            comStub.dataOut -> frameAccumulator.dataIn
            frameAccumulator.dataReturnOut -> comStub.dataReturnIn
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

} # end ComCcsds
