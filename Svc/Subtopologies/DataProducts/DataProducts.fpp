module DataProducts {

    # DataProducts subtopology expanded from Templates.DataProducts using the
    # project-overridable values in DataProductsConfig
    expand Templates.DataProducts(
        constant DataProductsConfig.BASE_ID
        constant {
            dpCat = {
                queueSize = DataProductsConfig.QueueSizes.dpCat
                stackSize = DataProductsConfig.StackSizes.dpCat
                $priority = DataProductsConfig.Priorities.dpCat
                cpuAffinity = DataProductsConfig.CpuAffinities.dpCat
            }
            dpMgr = {
                queueSize = DataProductsConfig.QueueSizes.dpMgr
                stackSize = DataProductsConfig.StackSizes.dpMgr
                $priority = DataProductsConfig.Priorities.dpMgr
                cpuAffinity = DataProductsConfig.CpuAffinities.dpMgr
            }
            dpWriter = {
                queueSize = DataProductsConfig.QueueSizes.dpWriter
                stackSize = DataProductsConfig.StackSizes.dpWriter
                $priority = DataProductsConfig.Priorities.dpWriter
                cpuAffinity = DataProductsConfig.CpuAffinities.dpWriter
            }
            dpBufferAccumulator = {
                queueSize = DataProductsConfig.QueueSizes.dpBufferAccumulator
                stackSize = DataProductsConfig.StackSizes.dpBufferAccumulator
                $priority = DataProductsConfig.Priorities.dpBufferAccumulator
                cpuAffinity = DataProductsConfig.CpuAffinities.dpBufferAccumulator
            }
            bufferAccumulatorAllocatorId = DataProductsConfig.BufferAccumulator.allocatorId
            bufferAccumulatorMaxNumBuffers = DataProductsConfig.BufferAccumulator.maxNumBuffers
            bufferManagerId = DataProductsConfig.BuffMgr.dpBufferManagerId
            bufferStoreSize = DataProductsConfig.BuffMgr.dpBufferStoreSize
            bufferStoreCount = DataProductsConfig.BuffMgr.dpBufferStoreCount
            dpDir = DataProductsConfig.Paths.dpDir
            dpState = DataProductsConfig.Paths.dpState
        }
    )

} # end DataProducts Subtopology
