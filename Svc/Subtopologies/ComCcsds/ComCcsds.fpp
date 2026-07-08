module ComCcsds {

    # ----------------------------------------------------------------------
    # Passive Components
    # ----------------------------------------------------------------------
    instance comStub: Svc.ComStub base id ComCcsdsConfig.BASE_ID + 0x00000

    # This subtopology composes the SpacePacket packet layer with the CcsdsFraming
    # TM/TC transfer frame layer to form the full CCSDS communications stack.
    topology FramingSubtopology {
        # Usage Note:
        #
        # When importing this subtopology, users shall establish 5 port connections with a component implementing
        # the Svc.Com (Svc/Interfaces/Com.fpp) interface. They are as follows:
        #
        # 1) Outputs:
        #     - CcsdsFraming.framer.dataOut                 -> [Svc.Com].dataIn
        #     - CcsdsFraming.frameAccumulator.dataReturnOut -> [Svc.Com].dataReturnIn
        # 2) Inputs:
        #     - [Svc.Com].dataReturnOut -> CcsdsFraming.framer.dataReturnIn
        #     - [Svc.Com].comStatusOut  -> CcsdsFraming.framer.comStatusIn
        #     - [Svc.Com].dataOut       -> CcsdsFraming.frameAccumulator.dataIn

        # Packet layer (router, ComQueue, space packet framer/deframer, buffer manager)
        import SpacePacket.FramingSubtopology

        # TM/TC transfer frame layer (TM framer, frame accumulator, TC deframer)
        import CcsdsFraming.Subtopology

        connections Downlink {
            # SpacePacket aggregator <-> TmFramer
            SpacePacket.aggregator.dataOut     -> CcsdsFraming.framer.dataIn
            CcsdsFraming.framer.dataReturnOut  -> SpacePacket.aggregator.dataReturnIn

            # ComStatus
            CcsdsFraming.framer.comStatusOut   -> SpacePacket.aggregator.comStatusIn
            # (Outgoing) Framer <-> ComInterface connections shall be established by the user
        }

        connections Uplink {
            # (Incoming) ComInterface <-> FrameAccumulator connections shall be established by the user
            # FrameAccumulator buffer allocations
            CcsdsFraming.frameAccumulator.bufferDeallocate -> SpacePacket.commsBufferManager.bufferSendIn
            CcsdsFraming.frameAccumulator.bufferAllocate   -> SpacePacket.commsBufferManager.bufferGetCallee

            # TcDeframer <-> SpacePacketDeframer
            CcsdsFraming.tcDeframer.dataOut               -> SpacePacket.spacePacketDeframer.dataIn
            SpacePacket.spacePacketDeframer.dataReturnOut -> CcsdsFraming.tcDeframer.dataReturnIn
        }
    } # end FramingSubtopology

    # This subtopology uses FramingSubtopology with a ComStub component for Com Interface
    topology Subtopology {
        import FramingSubtopology

        instance comStub

        connections ComStub {
            # Framer <-> ComStub (Downlink)
            CcsdsFraming.framer.dataOut -> comStub.dataIn
            comStub.dataReturnOut       -> CcsdsFraming.framer.dataReturnIn
            comStub.comStatusOut        -> CcsdsFraming.framer.comStatusIn

            # ComStub <-> FrameAccumulator (Uplink)
            comStub.dataOut -> CcsdsFraming.frameAccumulator.dataIn
            CcsdsFraming.frameAccumulator.dataReturnOut -> comStub.dataReturnIn
        }

        # ----------------------------------------------------------------------
        # Topology ports
        # ----------------------------------------------------------------------

        # Command routing
        @ Output port sending routed command packets to the command dispatcher
        port commandOut         = SpacePacket.fprimeRouter.commandOut

        @ Input port receiving command response messages back into the router
        port cmdResponseIn      = SpacePacket.fprimeRouter.cmdResponseIn

        @ Output port sending uplinked file packets to the file handling stack
        port fileUplinkOut          = SpacePacket.fprimeRouter.fileOut

        @ Input port receiving back buffer ownership from the file handling stack
        port fileUplinkReturnIn = SpacePacket.fprimeRouter.fileBufferReturnIn

        # Telemetry/events/file queuing (array ports - index at connection site)
        @ Input port array for queueing Fw::ComBuffers
        port comPacketQueueIn = SpacePacket.comQueue.comPacketQueueIn

        @ Input port array for queueing Fw::Buffers
        port bufferQueueIn    = SpacePacket.comQueue.bufferQueueIn

        @ Output port array returning ownership of Fw::Buffers to their original sender after dequeuing
        port bufferReturnOut  = SpacePacket.comQueue.bufferReturnOut

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
        port commsBufferGetCallee = SpacePacket.commsBufferManager.bufferGetCallee

        @ Input port for deallocating Fw::Buffers back into the comms buffer pool
        port commsBufferSendIn    = SpacePacket.commsBufferManager.bufferSendIn

        # Scheduling
        @ Input port for scheduling ComQueue telemetry output
        port comQueueRun          = SpacePacket.comQueue.run

        @ Rate-group driven timeout to flush the ComAggregator buffer
        port aggregatorTimeout    = SpacePacket.aggregator.timeout

        @ Input port triggering commsBufferManager telemetry output
        port bufferManagerSchedIn = SpacePacket.commsBufferManager.schedIn

    } # end Subtopology

} # end ComCcsds
