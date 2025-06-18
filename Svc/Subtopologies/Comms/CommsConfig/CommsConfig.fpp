module CommsConfig {
    #Base ID for the CDHCore Subtopology, all components are offsets from this base ID
    constant BASE_ID = 0x7000
    
    module QueueSizes {
        constant comQueue    = 10
        constant cmdSeq      = 10
    }
    

    module StackSizes {
        constant comQueue   = 64 * 1024
        constant cmdSeq    = 64 * 1024
    }

    module Priorities {
        constant comQueue   = 101
        constant cmdSeq     = 100
    }
}