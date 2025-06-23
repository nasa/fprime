module ComFprimeConfig {
    #Base ID for the ComFprime Subtopology, all components are offsets from this base ID
    constant BASE_ID = 0x7000
    
    module QueueSizes {
        constant comQueue    = 10
        constant cmdSeq      = 10
    }
    
    module StackSizes {
        constant comQueue   = 64 * 1024
        constant cmdSeq    = 64 * 1024
        constant comDriver = 100
    }

    module Priorities {
        constant comQueue   = 101
        constant cmdSeq     = 100
        constant comDriver  = 100
    }

    # Queue configuration constants
    module QueueDepths {
        constant events      = 100             # Event queue depth
        constant tlm         = 500             # Telemetry queue depth  
        constant file        = 100             # File downlink queue depth
    }

    module QueuePriorities {
        constant events      = 0               # Highest priority for events
        constant tlm         = 2               # Lower priority for telemetry
        constant file        = 1               # Medium priority for file transfers
    }

    # Buffer management constants
    module BufferMgr {
        constant cmdSeqBuffer           = 5 * 1024  # 5KB for command sequencer buffer
        constant frameAccumulator       = 2048      # 2KB frame accumulator buffer
        constant commsBufferStore       = 2048      # 2KB communications buffer store
        constant commsFileBufferStore   = 3000      # 3KB file buffer store
        constant commsBufferCount       = 20        # Number of comms buffers
        constant commsFileBufferQueue   = 30        # File queue buffer count
        constant commsBufferManager     = 200       # Buffer manager identifier
    }
}
