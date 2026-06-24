module ComFprimeConfig {
    #Base ID for the ComFprime Subtopology, all components are offsets from this base ID
    constant BASE_ID = 0x03000000
    
    module QueueSizes {
        constant comQueue    = 3
    }
    
    module StackSizes {
        constant comQueue   = 64 * 1024
    }

    module Priorities {
        constant comQueue   = 101
    }

    # Queue configuration constants
    module QueueDepths {
        constant events      = 10            
        constant tlm         = 25            
        constant file        = 1           
    }

    module QueuePriorities {
        constant events      = 0              
        constant tlm         = 2              
        constant file        = 1             
    }

    # Buffer management constants
    module BuffMgr {
        constant frameAccumulatorSize  = 256     
        constant commsBuffSize         = 140      
        constant commsFileBuffSize     = 0      
        constant commsBuffCount        = 3      
        constant commsFileBuffCount    = 0        
        constant commsBuffMgrId        = 200      
    }
}
