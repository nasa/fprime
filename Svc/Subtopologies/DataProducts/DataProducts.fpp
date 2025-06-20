module DataProducts{

    # ----------------------------------------------------------------------
    # Active Components
    # ----------------------------------------------------------------------
    
    instance dpCat: Svc.DpCatalog base id DataProductsConfig.BASE_ID + 0x0100 \
        queue size DataProductsConfig.QueueSizes.dpCat \
        stack size DataProductsConfig.StackSizes.dpCat \
        priority DataProductsConfig.Priorities.dpCat \
    {
        phase Fpp.ToCpp.Phases.configComponents """
            Fw::FileNameString dpDir("./DpCat");
            Fw::FileNameString dpState("./DpCat/DpState.dat");
            Os::FileSystem::createDirectory(dpDir.toChar());
            DataProducts::dpCat.configure(&dpDir,1,dpState,0, DataProducts::Allocation::mallocator);
        """
    }

    instance dpMgr: Svc.DpManager base id DataProductsConfig.BASE_ID + 0x0200 \
        queue size DataProductsConfig.QueueSizes.dpMgr \
        stack size DataProductsConfig.StackSizes.dpMgr \
        priority DataProductsConfig.Priorities.dpMgr

    instance dpWriter: Svc.DpWriter base id DataProductsConfig.BASE_ID + 0x0300 \
        queue size DataProductsConfig.QueueSizes.dpWriter \
        stack size DataProductsConfig.StackSizes.dpWriter \
        priority DataProductsConfig.Priorities.dpWriter \
    {
        phase Fpp.ToCpp.Phases.configComponents """
            DataProducts::dpWriter.configure(dpDir);
        """
    }
    
    # ----------------------------------------------------------------------
    # Passive Components
    # ----------------------------------------------------------------------
    
    instance dpBufferManager: Svc.BufferManager base id 0x4A00 \ 
    {
        phase Fpp.ToCpp.Phases.configConstants """
        enum {
            DP_BUFFER_MANAGER_STORE_SIZE  = 10000,
            DP_BUFFER_MANAGER_STORE_COUNT = 10,
            DP_BUFFER_MANAGER_ID          = 300
        };
        """
        phase Fpp.ToCpp.Phases.configComponents """
        memset(&DataProducts::BufferManagerBins::bins, 0, sizeof(DataProducts::BufferManagerBins::bins));
        DataProducts::BufferManagerBins::bins.bins[0].bufferSize = ConfigConstants::DataProducts_dpBufferManager::DP_BUFFER_MANAGER_STORE_SIZE;
        DataProducts::BufferManagerBins::bins.bins[0].numBuffers  = ConfigConstants::DataProducts_dpBufferManager::DP_BUFFER_MANAGER_STORE_COUNT;
        DataProducts::dpBufferManager.setup(
            ConfigConstants::DataProducts_dpBufferManager::DP_BUFFER_MANAGER_ID,
            0,
            DataProducts::Allocation::mallocator,
            DataProducts::BufferManagerBins::bins
        );
        """
        phase Fpp.ToCpp.Phases.tearDownComponents """
        DataProducts::dpBufferManager.deallocateBuffer(DataProducts::Allocation::mallocator);
        """
    }
    topology Subtopology {
        #Active Components
        instance dpCat
        instance dpMgr
        instance dpWriter

        #Passive Components
        instance dpBufferManager

        connections DataProducts {
            # DpMgr and DpWriter connections. Have explicit port indexes for demo
            dpMgr.bufferGetOut[0] -> dpBufferManager.bufferGetCallee
            dpMgr.productSendOut[0] -> dpWriter.bufferSendIn
            dpWriter.deallocBufferSendOut -> dpBufferManager.bufferSendIn
        }
    } # end topology
} # end DataProducts Subtopology
