module DataProducts{

    # ----------------------------------------------------------------------
    # Active Components
    # ----------------------------------------------------------------------
    
    instance dpCat: Svc.DpCatalog base id DataProductsConfig.BASE_ID + 0x00000 \
        queue size DataProductsConfig.QueueSizes.dpCat \
        stack size DataProductsConfig.StackSizes.dpCat \
        priority DataProductsConfig.Priorities.dpCat \
    {
        phase Fpp.ToCpp.Phases.configComponents """
            Fw::FileNameString dpDir(DataProductsConfig::Paths::dpDir);
            Fw::FileNameString dpState(DataProductsConfig::Paths::dpState);
            Os::FileSystem::createDirectory(dpDir.toChar());
            DataProducts::dpCat.configure(&dpDir,1,dpState,0, DataProducts::Allocation::memAllocator);
        """
    }

    instance dpMgr: Svc.DpManager base id DataProductsConfig.BASE_ID + 0x01000 \
        queue size DataProductsConfig.QueueSizes.dpMgr \
        stack size DataProductsConfig.StackSizes.dpMgr \
        priority DataProductsConfig.Priorities.dpMgr

    instance dpWriter: Svc.DpWriter base id DataProductsConfig.BASE_ID + 0x02000 \
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
    
    instance dpBufferManager: Svc.BufferManager base id DataProductsConfig.BASE_ID + 0x03000 \ 
    {
        phase Fpp.ToCpp.Phases.configObjects """
        Svc::BufferManager::BufferBins bins;
        """
        phase Fpp.ToCpp.Phases.configComponents """
        memset(&ConfigObjects::DataProducts_dpBufferManager::bins, 0, sizeof(ConfigObjects::DataProducts_dpBufferManager::bins));
        ConfigObjects::DataProducts_dpBufferManager::bins.bins[0].bufferSize = DataProductsConfig::BuffMgr::dpBufferStoreSize;
        ConfigObjects::DataProducts_dpBufferManager::bins.bins[0].numBuffers = DataProductsConfig::BuffMgr::dpBufferStoreCount;
        DataProducts::dpBufferManager.setup(
            DataProductsConfig::BuffMgr::dpBufferManagerId,
            0,
            DataProducts::Allocation::memAllocator,
            ConfigObjects::DataProducts_dpBufferManager::bins
        );
        """
        phase Fpp.ToCpp.Phases.tearDownComponents """
        DataProducts::dpCat.shutdown();
        DataProducts::dpBufferManager.cleanup();
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
