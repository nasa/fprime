module ComLogTSplitConfig {
    
    module QueueSizes {
        constant comLog      = 10
        constant comSplitter = 10
    }
    
    module StackSizes {
        constant comLog      = 64 * 1024
        constant comSplitter = 64 * 1024
    }

    module Priorities {
        constant comLog      = 95
        constant comSplitter = 96
    }
}
