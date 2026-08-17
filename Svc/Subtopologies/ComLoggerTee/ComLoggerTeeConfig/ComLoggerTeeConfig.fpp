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
        constant comLog = Os.TASK_DEFAULT
    }
}
