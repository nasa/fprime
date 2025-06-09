module DataProducts{

    enum Ports_ComBufferQueue {
        FILE_DOWNLINK
    }

    # ----------------------------------------------------------------------
    # Active Components
    # ----------------------------------------------------------------------
    
    instance dpCat: Svc.DpCatalog base id DataProductsConfig.BASE_ID + 0x0100 \
        queue size DataProductsConfig.QueueSizes.dpCat \
        stack size DataProductsConfig.StackSizes.dpCat \
        priority DataProductsConfig.Priorities.dpCat 

    instance dpMgr: Svc.DpManager base id DataProductsConfig.BASE_ID + 0x0200 \
        queue size DataProductsConfig.QueueSizes.dpMgr \
        stack size DataProductsConfig.StackSizes.dpMgr \
        priority DataProductsConfig.Priorities.dpMgr

    instance dpWriter: Svc.DpWriter base id DataProductsConfig.BASE_ID + 0x0300 \
        queue size DataProductsConfig.QueueSizes.dpWriter \
        stack size DataProductsConfig.StackSizes.dpWriter \
        priority DataProductsConfig.Priorities.dpWriter

    # ----------------------------------------------------------------------
    # Queued Components
    # ----------------------------------------------------------------------
    #none

    
    # ----------------------------------------------------------------------
    # Passive Components
    # ----------------------------------------------------------------------

    instance dpBufferManager: Svc.BufferManager base id 0x4A00
    
    topology Subtopology {
        #Active Components
        instance dpCat
        instance dpMgr
        instance dpWriter

        #Passive Components
        instance dpBufferManager

        # Subtopology imports
        import Comms.Subtopology

        connections DataProducts {
            # DpMgr and DpWriter connections. Have explicit port indexes for demo
            dpMgr.bufferGetOut[0] -> dpBufferManager.bufferGetCallee
            dpMgr.productSendOut[0] -> dpWriter.bufferSendIn
            dpWriter.deallocBufferSendOut -> dpBufferManager.bufferSendIn

            # Component DP connections

        }
    } # end topology
} # end Comms Subtopology
