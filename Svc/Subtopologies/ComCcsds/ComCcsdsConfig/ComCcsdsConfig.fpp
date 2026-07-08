module ComCcsdsConfig {
    #Base ID for the ComCcsds Subtopology, all components are offsets from this base ID
    constant BASE_ID = 0x02000000
    
    module QueueSizes {
        constant comQueue    = 50
        constant aggregator  = 10
    }
    
    module StackSizes {
        constant comQueue   = 64 * 1024
        constant aggregator = 64 * 1024
    }

    module Priorities {
        constant aggregator = 30
        constant comQueue   = 29
    }

    # Queue configuration constants
    module QueueDepths {
        constant events      = 200             
        constant tlm         = 500            
        constant file        = 100            
    }

    module QueuePriorities {
        constant events      = 0                 
        constant tlm         = 2                 
        constant file        = 1                   
    }

    # Buffer management constants
    module BuffMgr {
        constant frameAccumulatorSize  = 2048     
        constant commsBuffSize         = 2048      
        constant commsFileBuffSize     = 3000      
        constant commsBuffCount        = 20        
        constant commsFileBuffCount    = 30       
        constant commsBuffMgrId        = 200      
    }
}
