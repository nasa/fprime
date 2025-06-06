module Comms {

    enum Ports_ComPacketQueue {
        EVENTS,
        TELEMETRY
    }

    # ----------------------------------------------------------------------
    # Active Components
    # ----------------------------------------------------------------------
    instance comQueue: Svc.ComQueue base id CommsConfig.BASE_ID + 0x0100 \
        queue size CommsConfig.QueueSizes.comQueue\
        stack size CommsConfig.StackSizes.comQueue\
        priority CommsConfig.Priorities.comQueue


    # ----------------------------------------------------------------------
    # Queued Components
    # ----------------------------------------------------------------------
    #none

    
    # ----------------------------------------------------------------------
    # Passive Components
    # ----------------------------------------------------------------------
    instance commsBufferManager: Svc.BufferManager base id CommsConfig.BASE_ID + 0x0500 \

    instance frameAccumulator: Svc.FrameAccumulator base id CommsConfig.BASE_ID + 0x0600 \
  
    instance deframer: Svc.FprimeDeframer base id CommsConfig.BASE_ID + 0x0700 \

    instance fprimeFramer: Svc.FprimeFramer base id CommsConfig.BASE_ID + 0x0800 \

    instance fprimeRouter: Svc.FprimeRouter base id CommsConfig.BASE_ID + 0x0900 \
    
    instance comStub: Svc.ComStub base id CommsConfig.BASE_ID + 0x0A00 \

    instance comDriver: Drv.TcpClient base id CommsConfig.BASE_ID + 0x0B00 \

    topology Subtopology {
        #Active Components
        instance comQueue

        #Passive Components
        instance commsBufferManager
        instance frameAccumulator
        instance deframer
        instance fprimeFramer
        instance fprimeRouter
        instance comStub
        instance comDriver

        import CDHCore.Subtopology

        connections Downlink {
            # Inputs to ComQueue (events, telemetry, file)
            CDHCore.events.PktSend        -> comQueue.comPacketQueueIn[Ports_ComPacketQueue.EVENTS]
            CDHCore.tlmSend.PktSend            -> comQueue.comPacketQueueIn[Ports_ComPacketQueue.TELEMETRY]
            #fileDownlink.bufferSendOut -> comQueue.bufferQueueIn[Ports_ComBufferQueue.FILE_DOWNLINK] comQueue.bufferReturnOut[Ports_ComBufferQueue.FILE_DOWNLINK] -> fileDownlink.bufferReturn
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
            # Router <-> CmdDispatcher/FileUplink
            fprimeRouter.commandOut  -> CDHCore.cmdDisp.seqCmdBuff
            CDHCore.cmdDisp.seqCmdStatus     -> fprimeRouter.cmdResponseIn
            #fprimeRouter.fileOut     -> fileUplink.bufferSendIn
            #fileUplink.bufferSendOut -> fprimeRouter.fileBufferReturnIn
    }

    } # end topology
} # end Comms Subtopology