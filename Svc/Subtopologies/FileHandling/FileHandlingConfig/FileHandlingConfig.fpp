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
        constant fileUplink    = 24
        constant fileDownlink  = 23
        constant fileManager   = 22
        constant prmDb         = 21
    }

    module CpuAffinities {
        constant fileUplink    = Os.TASK_DEFAULT
        constant fileDownlink  = Os.TASK_DEFAULT
        constant fileManager   = Os.TASK_DEFAULT
        constant prmDb         = Os.TASK_DEFAULT
    }

    # File downlink configuration constants
    module DownlinkConfig {
        constant cooldown       = 1000         # File downlink cooldown in ms
        constant cycleTime      = 1000         # File downlink cycle time in ms
        constant fileQueueDepth = 10           # File downlink queue depth
    }

    # File-access sandbox base directory.
    # The value is applied in FileHandling.fpp's configComponents phases (as a C++
    # string literal, because FPP string constants live in this INTERFACE config
    # library's unlinked FppConstantsAc.cpp and cannot be referenced from topology
    # autocode). Keep this comment and the literals in FileHandling.fpp in sync.
    #   Default "." confines all ground-commanded file reads/writes (FileUplink,
    #   FileDownlink, PrmDb PRM_LOAD_FILE) to the deployment working-directory subtree,
    #   rejecting "../" traversal and absolute-path escapes. A deployment that truly
    #   needs unrestricted access must set the literals to "/" as a deliberate opt-in.
}
