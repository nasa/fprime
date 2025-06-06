module FileHandling{

    enum Ports_ComPacketQueue {
        EVENTS,
        TELEMETRY
    }

    # ----------------------------------------------------------------------
    # Active Components
    # ----------------------------------------------------------------------
    
    #ex:
    #instance comQueue: Svc.ComQueue base id CommsConfig.BASE_ID + 0x0100 \
    #    queue size CommsConfig.QueueSizes.comQueue \
    #    stack size CommsConfig.StackSizes.comQueue \
    #    priority CommsConfig.Priorities.comQueue



    # ----------------------------------------------------------------------
    # Queued Components
    # ----------------------------------------------------------------------
    #none

    
    # ----------------------------------------------------------------------
    # Passive Components
    # ----------------------------------------------------------------------
    
    #ex:
    #instance commsBufferManager: Svc.BufferManager base id CommsConfig.BASE_ID + 0x0500 \


  @ Communications driver. May be swapped with other comm drivers like UART
    instance comDriver: Drv.TcpClient base id CommsConfig.BASE_ID + 0x0B00 \

    topology Subtopology {
        #Active Components

        #Passive Components

        # Subtopology imports
        # DataProducts does not exist yet, but we will need it for the file handling subtopology
        #import DataProducts.Subtopology

        connections FileHandling {
            # Data Products
            dpCat.fileOut             -> fileDownlink.SendFile
            fileDownlink.FileComplete -> dpCat.fileDone

            # File Downlink <-> ComQueue
            fileDownlink.bufferSendOut -> comQueue.bufferQueueIn[Ports_ComBufferQueue.FILE_DOWNLINK]
            comQueue.bufferReturnOut[Ports_ComBufferQueue.FILE_DOWNLINK] -> fileDownlink.bufferReturn

            # Router <-> FileUplink
            fprimeRouter.fileOut     -> fileUplink.bufferSendIn
            fileUplink.bufferSendOut -> fprimeRouter.fileBufferReturnIn
        }
    } # end topology
} # end Comms Subtopology
