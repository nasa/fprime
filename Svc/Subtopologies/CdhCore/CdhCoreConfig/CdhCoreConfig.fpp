module CdhCoreConfig {
    #Base ID for the CdhCore Subtopology, all components are offsets from this base ID
    constant BASE_ID = 0x01000000
    
    module QueueSizes {
        constant cmdDisp     = 10
        constant events      = 10
        constant tlmSend     = 10
        constant $health     = 25
    }
    

    module StackSizes {
        constant cmdDisp     = 64 * 1024
        constant events      = 64 * 1024
        constant tlmSend     = 64 * 1024
    }

    module Priorities {
        constant cmdDisp     = 35
        constant $health     = 24
        constant events      = 23
        constant tlmSend     = 22

    }

    module CpuAffinities {
        # -1 casts to Os::Task::TASK_DEFAULT (no CPU pinning)
        constant TASK_DEFAULT = -1
        constant cmdDisp     = TASK_DEFAULT
        constant events      = TASK_DEFAULT
        constant tlmSend     = TASK_DEFAULT
    }
}
