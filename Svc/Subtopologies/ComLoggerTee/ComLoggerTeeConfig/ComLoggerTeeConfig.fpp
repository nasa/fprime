module ComLoggerTeeConfig {
    
    module QueueSizes {
        constant comLog = 10
    }
    
    module StackSizes {
        constant comLog = 64 * 1024
    }

    module Priorities {
        constant comLog = 18
    }

    module CpuAffinities {
        # -1 casts to Os::Task::TASK_DEFAULT (no CPU pinning)
        constant TASK_DEFAULT = -1
        constant comLog = TASK_DEFAULT
    }
}
