module Templates {

    @ Generic configuration for an active component instance definition
    struct ActiveComponentConfig {
        queueSize: FwSizeType
        stackSize: FwSizeType
        $priority: FwIndexType
        cpuAffinity: FwIndexType
    } default {
        queueSize = 10
        stackSize = 64 * 1024
        $priority = 99
        cpuAffinity = Os.TASK_DEFAULT
    }

    @ Configuration for the DataProducts subtopology template
    struct DataProductsConfig {
        dpCat: ActiveComponentConfig
        dpMgr: ActiveComponentConfig
        dpWriter: ActiveComponentConfig
        dpBufferAccumulator: ActiveComponentConfig

        @ Memory identifier used by dpBufferAccumulator to allocate its queue
        bufferAccumulatorAllocatorId: FwEnumStoreType
        @ Maximum number of buffers held by dpBufferAccumulator
        bufferAccumulatorMaxNumBuffers: FwSizeType

        @ Manager identifier used by dpBufferManager for buffer checking
        bufferManagerId: U16
        @ Size of each data product buffer
        bufferStoreSize: FwSizeType
        @ Number of data product buffers
        bufferStoreCount: U16

        @ Directory where data products are written and cataloged
        dpDir: string size FileNameStringSize
        @ File storing the catalog transmit state
        dpState: string size FileNameStringSize
    } default {
        dpCat = { queueSize = 10, stackSize = 64 * 1024, $priority = 24, cpuAffinity = Os.TASK_DEFAULT }
        dpMgr = { queueSize = 10, stackSize = 64 * 1024, $priority = 23, cpuAffinity = Os.TASK_DEFAULT }
        dpWriter = { queueSize = 10, stackSize = 64 * 1024, $priority = 22, cpuAffinity = Os.TASK_DEFAULT }
        dpBufferAccumulator = { queueSize = 10, stackSize = 64 * 1024, $priority = 21, cpuAffinity = Os.TASK_DEFAULT }
        bufferAccumulatorAllocatorId = 301
        bufferAccumulatorMaxNumBuffers = 10
        bufferManagerId = 300
        bufferStoreSize = 10000
        bufferStoreCount = 10
        dpDir = "./DpCat"
        dpState = "./DpCat/DpState.dat"
    }

}
