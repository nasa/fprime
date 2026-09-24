module RecordedComConfig {
    # Base ID for the RecordedCom Subtopology, all components are offsets from this base ID
    constant BASE_ID = 0x08000000

    # Queue sizes for active components
    module QueueSizes {
        constant comLogger = 20
        constant dpMgr = 15
        constant dpWriter = 10
    }

    # Stack sizes for active components
    module StackSizes {
        constant comLogger = 64 * 1024
        constant dpMgr = 64 * 1024
        constant dpWriter = 64 * 1024
    }

    # Priorities for active components
    module Priorities {
        constant comLogger = 25
        constant dpMgr = 24
        constant dpWriter = 23
    }

    # CPU affinities for active components
    module CpuAffinities {
        constant comLogger = Os.TASK_DEFAULT
        constant dpMgr = Os.TASK_DEFAULT
        constant dpWriter = Os.TASK_DEFAULT
    }

    # Buffer manager configuration
    module BuffMgr {
        constant dpBufferStoreSize = 8192
        constant dpBufferStoreCount = 20
        constant dpBufferManagerId = 302
    }

    # ComLogger configuration
    module ComLogger {
        constant packetsPerContainer = 10
        constant enabled = false
        constant defaultPriority = 5
        constant flushTimeout = 10 
    }

    # File paths
    module Paths {
        constant dpDir = "./DpCat"
    }
}
