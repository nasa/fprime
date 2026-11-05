module DpCompression {

    # ----------------------------------------------------------------------
    # Passive Components
    # ----------------------------------------------------------------------

    instance dpCompressProc: Svc.DpCompressProc base id DpCompressionConfig.BASE_ID + 0x00000

    instance dpZLibCompressor: Svc.DpZLibCompressor base id DpCompressionConfig.BASE_ID + 0x01000

    instance dpZLibCompressorBufferManager: Svc.BufferManager base id DpCompressionConfig.BASE_ID + 0x02000 \ 
    {
        phase Fpp.ToCpp.Phases.configObjects """
        Svc::BufferManager::BufferBins bins;
        """
        phase Fpp.ToCpp.Phases.configComponents """
        memset(&ConfigObjects::DpCompression_dpZLibCompressorBufferManager::bins, 0, sizeof(ConfigObjects::DpCompression_dpZLibCompressorBufferManager::bins));
        ConfigObjects::DpCompression_dpZLibCompressorBufferManager::bins.bins[0].bufferSize = DpCompressionConfig::ZLibBuffMgr::compressionBufferStoreSize;
        ConfigObjects::DpCompression_dpZLibCompressorBufferManager::bins.bins[0].numBuffers = DpCompressionConfig::ZLibBuffMgr::compressionBufferStoreCount;
        ConfigObjects::DpCompression_dpZLibCompressorBufferManager::bins.bins[1].bufferSize = DpCompressionConfig::ZLibBuffMgr::allocBufferStoreSize;
        ConfigObjects::DpCompression_dpZLibCompressorBufferManager::bins.bins[1].numBuffers = DpCompressionConfig::ZLibBuffMgr::allocBufferStoreCount;
        DpCompression::dpZLibCompressorBufferManager.setup(
            DpCompressionConfig::ZLibBuffMgr::dpZLibCompressorBufferManagerId,
            0,
            DpCompression::Allocation::memAllocator,
            ConfigObjects::DpCompression_dpZLibCompressorBufferManager::bins
        );
        """
        phase Fpp.ToCpp.Phases.tearDownComponents """
        DpCompression::dpZLibCompressorBufferManager.cleanup();
        """
    }

    topology Subtopology {

        #Passive Components
        instance dpCompressProc
        instance dpZLibCompressor
        instance dpZLibCompressorBufferManager
        

        connections DpCompression {
            dpCompressProc.compressChunk -> dpZLibCompressor.compressChunk
            dpZLibCompressor.bufferCompressionGet -> dpZLibCompressorBufferManager.bufferGetCallee
            dpZLibCompressor.bufferCompressionReturn -> dpZLibCompressorBufferManager.bufferSendIn

            dpZLibCompressor.bufferZLibGet -> dpZLibCompressorBufferManager.bufferGetCallee
            dpZLibCompressor.bufferZLibReturn -> dpZLibCompressorBufferManager.bufferSendIn
        }
        # ----------------------------------------------------------------------
        # Topology ports
        # ----------------------------------------------------------------------

        @ Input port for Data Product Compression Processing Requests
        port dpCompressProcIn = dpCompressProc.procRequest

        @ Input port for scheduling dpZLibCompressorBufferManager telemetry output
        port dpZLibBufferManagerSchedIn = dpZLibCompressorBufferManager.schedIn

    } # end topology
} # end DpCompression Subtopology
