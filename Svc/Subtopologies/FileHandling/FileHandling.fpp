module FileHandling{

  enum Ports_ComBufferQueue {
    FILE_DOWNLINK
  }

    # ----------------------------------------------------------------------
    # Active Components
    # ----------------------------------------------------------------------
    
    #ex:
    #instance comQueue: Svc.ComQueue base id CommsConfig.BASE_ID + 0x0100 \
    #    queue size CommsConfig.QueueSizes.comQueue \
    #    stack size CommsConfig.StackSizes.comQueue \
    #    priority CommsConfig.Priorities.comQueue

    instance fileUplink: Svc.FileUplink base id FileHandlingConfig.BASE_ID + 0x0100 \
        queue size FileHandlingConfig.QueueSizes.fileUplink \
        stack size FileHandlingConfig.StackSizes.fileUplink \
        priority FileHandlingConfig.Priorities.fileUplink 

    instance fileDownlink: Svc.FileDownlink base id FileHandlingConfig.BASE_ID + 0x0200 \
        queue size FileHandlingConfig.QueueSizes.fileDownlink \
        stack size FileHandlingConfig.StackSizes.fileDownlink \
        priority FileHandlingConfig.Priorities.fileDownlink

    instance fileManager: Svc.FileManager base id FileHandlingConfig.BASE_ID + 0x0300 \
        queue size FileHandlingConfig.QueueSizes.fileManager \
        stack size FileHandlingConfig.StackSizes.fileManager \
        priority FileHandlingConfig.Priorities.fileManager

    # ----------------------------------------------------------------------
    # Queued Components
    # ----------------------------------------------------------------------
    #none

    
    # ----------------------------------------------------------------------
    # Passive Components
    # ----------------------------------------------------------------------
    
    #ex:
    #instance commsBufferManager: Svc.BufferManager base id CommsConfig.BASE_ID + 0x0500 \

    topology Subtopology {
        #Active Components
        instance fileUplink
        instance fileDownlink
        instance fileManager

        #Passive Components

        # Subtopology imports
        # DataProducts does not exist yet, but we will need it for the file handling subtopology
        import DataProducts.Subtopology
        import Comms.Subtopology

        connections FileHandling {
            # Data Products
            DataProducts.dpCat.fileOut             -> fileDownlink.SendFile
            fileDownlink.FileComplete -> DataProducts.dpCat.fileDone

            # File Downlink <-> ComQueue
            fileDownlink.bufferSendOut -> Comms.comQueue.bufferQueueIn[Ports_ComBufferQueue.FILE_DOWNLINK]
            Comms.comQueue.bufferReturnOut[Ports_ComBufferQueue.FILE_DOWNLINK] -> fileDownlink.bufferReturn

            # Router <-> FileUplink
            Comms.fprimeRouter.fileOut     -> fileUplink.bufferSendIn
            fileUplink.bufferSendOut -> Comms.fprimeRouter.fileBufferReturnIn
        }
    } # end topology
} # end Comms Subtopology
