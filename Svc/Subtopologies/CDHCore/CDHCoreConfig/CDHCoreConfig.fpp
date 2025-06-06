module CDHCoreConfig {
    #Base ID for the CDHCore Subtopology, all components are offsets from this base ID
    constant BASE_ID = 0x7000
    
    module QueueSizes {
        constant cmdDisp     = 10
        constant events      = 10
        constant tlmSend     = 10
        constant $health     = 25
    }
    

    module StackSizes {
        constant cmdDisp     = 64 * 1024
        constant events      = 10
        constant tlmSend     = 64 * 1024
    }

    module Priorities {
        constant cmdDisp     = 101
        constant $health     = 100
        constant events      = 99
        constant tlmSend     = 98
    }
}