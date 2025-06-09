module FileHandlingConfig {
    #Base ID for the CDHCore Subtopology, all components are offsets from this base ID
    constant BASE_ID = 0x9000
    
    module QueueSizes {
        constant fileUplink    = 10
        constant fileDownlink  = 10
        constant fileManager   = 10
    }
    

    module StackSizes {
        constant fileUplink    = 64 * 1024
        constant fileDownlink  = 64 * 1024
        constant fileManager   = 64 * 1024
    }

    module Priorities {
        constant fileUplink    = 101
        constant fileDownlink  = 100
        constant fileManager   = 99
    }
}