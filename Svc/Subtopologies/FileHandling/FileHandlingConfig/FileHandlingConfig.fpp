module FileHandlingConfig {
    #Base ID for the FileHandling Subtopology, all components are offsets from this base ID
    constant BASE_ID = 0x05000000
    
    module QueueSizes {
        constant fileUplink    = 10
        constant fileDownlink  = 10
        constant fileManager   = 10
        constant prmDb         = 10
    }
    
    module StackSizes {
        constant fileUplink    = 64 * 1024
        constant fileDownlink  = 64 * 1024
        constant fileManager   = 64 * 1024
        constant prmDb         = 64 * 1024
    }

    module Priorities {
        constant fileUplink    = 101
        constant fileDownlink  = 100
        constant fileManager   = 99
        constant prmDb         = 98
    }

    # File downlink configuration constants
    module DownlinkConfig {
        constant timeout        = 1000         # File downlink timeout in ms
        constant cooldown       = 1000         # File downlink cooldown in ms  
        constant cycleTime      = 1000         # File downlink cycle time in ms
        constant fileQueueDepth = 10           # File downlink queue depth
    }
}
