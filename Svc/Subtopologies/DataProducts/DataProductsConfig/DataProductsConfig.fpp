module DataProductsConfig {
    #Base ID for the CDHCore Subtopology, all components are offsets from this base ID
    constant BASE_ID = 0x9000
    
    module QueueSizes {
        constant dpCat    = 10
        constant dpMgr  = 10
        constant dpWriter   = 10
        constant dpBufferManager   = 10
    }
    

    module StackSizes {
        constant dpCat    = 64 * 1024
        constant dpMgr  = 64 * 1024
        constant dpWriter   = 64 * 1024
        constant dpBufferManager   = 64 * 1024
    }

    module Priorities {
        constant dpCat    = 101
        constant dpMgr  = 100
        constant dpWriter   = 99
        constant dpBufferManager  = 98
    }
}