module ComCcsdsSdls {

    # ----------------------------------------------------------------------
    # SDLS decryption instances
    # ----------------------------------------------------------------------

    instance sdlsDeframer: Svc.Ccsds.CcsdsSdlsDeframer base id ComCcsdsSdlsConfig.BASE_ID + 0x00000

    instance saRouter: Svc.Ccsds.SdlsSaRouter base id ComCcsdsSdlsConfig.BASE_ID + 0x01000

    # NOTE: the 'decryptor' instance is defined in the ComCcsdsSdlsConfig configuration
    # module, allowing projects to override the configuration and select a different
    # decryptor implementation. The default is Svc.Ccsds.ClearTextDecryptor (NO security).

    # This subtopology boxes the SDLS decryption layer: the SDLS deframer (SA extraction),
    # the SA router, and the default decryptor. It sits between the transfer frame layer
    # and the packet layer in the uplink path.
    topology SdlsDecryption {
        # Usage Note:
        #
        # When importing this subtopology, users shall establish the following external connections:
        #
        # 1) Upstream (transfer frame layer, e.g. ComCcsds.TmTcFraming):
        #     - [upstream].dataOut                       -> ComCcsdsSdls.SdlsDecryption.dataIn
        #     - ComCcsdsSdls.SdlsDecryption.dataReturnOut -> [upstream].dataReturnIn
        # 2) Downstream (packet layer, e.g. ComCcsds.SpacePacketFraming):
        #     - ComCcsdsSdls.SdlsDecryption.dataOut      -> [downstream].dataIn
        #     - [downstream].dataReturnOut               -> ComCcsdsSdls.SdlsDecryption.dataReturnIn

        instance sdlsDeframer
        instance saRouter
        instance decryptor

        connections Decryption {
            # CcsdsSdlsDeframer <-> SdlsSaRouter (decryption requests and returns)
            sdlsDeframer.decryptOut       -> saRouter.decryptIn
            saRouter.decryptOut           -> sdlsDeframer.decryptIn
            sdlsDeframer.decryptReturnOut -> saRouter.decryptReturnIn
            saRouter.bufferReturnOut      -> sdlsDeframer.bufferReturnIn

            # SdlsSaRouter <-> default decryptor
            saRouter.saDecryptOut[SdlsCfg.SaRouterPorts.PLAINTEXT_DECRYPTION]       -> decryptor.decryptIn
            decryptor.decryptOut           -> saRouter.saDecryptIn[SdlsCfg.SaRouterPorts.PLAINTEXT_DECRYPTION]
            saRouter.saDecryptReturnOut[SdlsCfg.SaRouterPorts.PLAINTEXT_DECRYPTION] -> decryptor.decryptReturnIn
            decryptor.bufferReturnOut      -> saRouter.saBufferReturnIn[SdlsCfg.SaRouterPorts.PLAINTEXT_DECRYPTION]
        }

        # ----------------------------------------------------------------------
        # Topology ports
        # ----------------------------------------------------------------------

        # Upstream boundary (transfer frame layer)
        @ Input port receiving TC-deframed SDLS frames from the transfer frame layer
        port dataIn        = sdlsDeframer.dataIn

        @ Output port returning ownership of uplinked buffers to the transfer frame layer
        port dataReturnOut = sdlsDeframer.dataReturnOut

        # Downstream boundary (packet layer)
        @ Output port sending decrypted data to the packet layer
        port dataOut       = sdlsDeframer.dataOut

        @ Input port receiving back ownership of decrypted buffers from the packet layer
        port dataReturnIn  = sdlsDeframer.dataReturnIn
    } # end SdlsDecryption

    # This subtopology composes the ComCcsds SpacePacketFraming packet layer and the
    # ComCcsds TmTcFraming transfer frame layer with the SdlsDecryption layer inserted
    # in between on the uplink path.
    topology FramingSubtopology {
        # Usage Note:
        #
        # When importing this subtopology, users shall establish 5 port connections with a component implementing
        # the Svc.Com (Svc/Interfaces/Com.fpp) interface. They are as follows:
        #
        # 1) Outputs:
        #     - ComCcsdsSdls.FramingSubtopology.dataOut       -> [Svc.Com].dataIn
        #     - ComCcsdsSdls.FramingSubtopology.dataReturnOut -> [Svc.Com].dataReturnIn
        # 2) Inputs:
        #     - [Svc.Com].dataReturnOut -> ComCcsdsSdls.FramingSubtopology.dataReturnIn
        #     - [Svc.Com].comStatusOut  -> ComCcsdsSdls.FramingSubtopology.comStatusIn
        #     - [Svc.Com].dataOut       -> ComCcsdsSdls.FramingSubtopology.dataIn

        # Packet layer (router, ComQueue, space packet framer/deframer, buffer manager)
        import ComCcsds.SpacePacketFraming

        # TM/TC transfer frame layer (TM framer, frame accumulator, TC deframer)
        import ComCcsds.TmTcFraming

        # SDLS decryption layer (SDLS deframer, SA router, decryptor)
        import SdlsDecryption

        connections Downlink {
            # SpacePacketFraming <-> TmTcFraming (downlink is not encrypted)
            ComCcsds.SpacePacketFraming.dataOut -> ComCcsds.TmTcFraming.dataIn
            ComCcsds.TmTcFraming.dataReturnOut  -> ComCcsds.SpacePacketFraming.dataReturnIn

            # ComStatus
            ComCcsds.TmTcFraming.comStatusOut -> ComCcsds.SpacePacketFraming.comStatusIn
            # (Outgoing) TmTcFraming <-> ComInterface connections shall be established by the user
        }

        connections Uplink {
            # (Incoming) ComInterface <-> TmTcFraming connections shall be established by the user
            # TmTcFraming buffer allocations
            ComCcsds.TmTcFraming.bufferDeallocate -> ComCcsds.SpacePacketFraming.bufferSendIn
            ComCcsds.TmTcFraming.bufferAllocate   -> ComCcsds.SpacePacketFraming.bufferGetCallee

            # TmTcFraming <-> SdlsDecryption (SDLS decryption step)
            ComCcsds.TmTcFraming.dataOut -> SdlsDecryption.dataIn
            SdlsDecryption.dataReturnOut -> ComCcsds.TmTcFraming.dataReturnIn

            # SdlsDecryption <-> SpacePacketFraming
            SdlsDecryption.dataOut                    -> ComCcsds.SpacePacketFraming.dataIn
            ComCcsds.SpacePacketFraming.dataReturnOut -> SdlsDecryption.dataReturnIn
        }

        # ----------------------------------------------------------------------
        # Topology ports (Svc.Com boundary)
        # ----------------------------------------------------------------------

        @ Output port sending TM transfer frames to the com interface
        port dataOut       = ComCcsds.framer.dataOut

        @ Input port receiving back ownership of transmitted frame buffers from the com interface
        port dataReturnIn  = ComCcsds.framer.dataReturnIn

        @ Input port receiving com status from the com interface
        port comStatusIn   = ComCcsds.framer.comStatusIn

        @ Input port receiving raw uplink data from the com interface
        port dataIn        = ComCcsds.frameAccumulator.dataIn

        @ Output port returning ownership of received uplink buffers to the com interface
        port dataReturnOut = ComCcsds.frameAccumulator.dataReturnOut
    } # end FramingSubtopology

    # This subtopology uses FramingSubtopology with a ComStub component for Com Interface
    topology Subtopology {
        import FramingSubtopology

        instance ComCcsds.comStub

        connections ComStub {
            # FramingSubtopology <-> ComStub (Downlink)
            FramingSubtopology.dataOut  -> ComCcsds.comStub.dataIn
            ComCcsds.comStub.dataReturnOut -> FramingSubtopology.dataReturnIn
            ComCcsds.comStub.comStatusOut  -> FramingSubtopology.comStatusIn

            # ComStub <-> FramingSubtopology (Uplink)
            ComCcsds.comStub.dataOut -> FramingSubtopology.dataIn
            FramingSubtopology.dataReturnOut -> ComCcsds.comStub.dataReturnIn
        }

        # ----------------------------------------------------------------------
        # Topology ports
        # ----------------------------------------------------------------------

        # Command routing
        @ Output port sending routed command packets to the command dispatcher
        port commandOut         = ComCcsds.fprimeRouter.commandOut

        @ Input port receiving command response messages back into the router
        port cmdResponseIn      = ComCcsds.fprimeRouter.cmdResponseIn

        @ Output port sending uplinked file packets to the file handling stack
        port fileUplinkOut          = ComCcsds.fprimeRouter.fileOut

        @ Input port receiving back buffer ownership from the file handling stack
        port fileUplinkReturnIn = ComCcsds.fprimeRouter.fileBufferReturnIn

        # Telemetry/events/file queuing (array ports - index at connection site)
        @ Input port array for queueing Fw::ComBuffers
        port comPacketQueueIn = ComCcsds.comQueue.comPacketQueueIn

        @ Input port array for queueing Fw::Buffers
        port bufferQueueIn    = ComCcsds.comQueue.bufferQueueIn

        @ Output port array returning ownership of Fw::Buffers to their original sender after dequeuing
        port bufferReturnOut  = ComCcsds.comQueue.bufferReturnOut

        # ComDriver interface (via ComStub)
        @ Input port receiving data read from the ByteStream driver
        port drvReceiveIn        = ComCcsds.comStub.drvReceiveIn

        @ Output port returning ownership of the buffer that came in on drvReceiveIn back to the driver
        port drvReceiveReturnOut = ComCcsds.comStub.drvReceiveReturnOut

        @ Output port sending framed data to the ByteStream driver for transmission
        port drvSendOut          = ComCcsds.comStub.drvSendOut

        @ Input port receiving the ready signal when the ByteStream driver has connected
        port drvConnected        = ComCcsds.comStub.drvConnected

        # Buffer management for ComDriver
        @ Input port for requesting (allocating) a new Fw::Buffer from the comms buffer pool
        port commsBufferGetCallee = ComCcsds.commsBufferManager.bufferGetCallee

        @ Input port for deallocating Fw::Buffers back into the comms buffer pool
        port commsBufferSendIn    = ComCcsds.commsBufferManager.bufferSendIn

        # Scheduling
        @ Input port for scheduling ComQueue telemetry output
        port comQueueRun          = ComCcsds.comQueue.run

        @ Rate-group driven timeout to flush the ComAggregator buffer
        port aggregatorTimeout    = ComCcsds.aggregator.timeout

        @ Input port triggering commsBufferManager telemetry output
        port bufferManagerSchedIn = ComCcsds.commsBufferManager.schedIn

    } # end Subtopology

} # end ComCcsdsSdls
