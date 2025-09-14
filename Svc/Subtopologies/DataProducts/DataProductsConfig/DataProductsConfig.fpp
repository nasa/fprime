module DataProductsConfig {
    #Base ID for the DataProducts Subtopology, all components are offsets from this base ID
    constant BASE_ID = 0x04000000
    
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

    # Buffer management constants
    module BuffMgr {
        constant dpBufferStoreSize  = 10000   
        constant dpBufferStoreCount = 10       
        constant dpBufferManagerId  = 300         
    }
    
    # Directory and file paths
    module Paths {
        constant dpDir    = "./DpCat"                
        constant dpState  = "./DpCat/DpState.dat"  
    }
}
