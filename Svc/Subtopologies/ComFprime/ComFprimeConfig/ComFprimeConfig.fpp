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

module ComFprime {
    # Communications driver. May be swapped out with other comm drivers like UART in this file
    # to use another driver in the Comms Subtopology
    instance comDriver: Drv.TcpClient base id ComFprimeConfig.BASE_ID + 0x0B00 \ 
    {
        phase Fpp.ToCpp.Phases.configComponents """
        if (state.hostname != nullptr && state.port != 0) {
            ComFprime::comDriver.configure(state.hostname, state.port);
        }
        """

        phase Fpp.ToCpp.Phases.startTasks """
        // Initialize socket client communication if and only if there is a valid specification
        if (state.hostname != nullptr && state.port != 0) {
            Os::TaskString name("ReceiveTask");
            ComFprime::comDriver.start(name, ComFprimeConfig::Priorities::comDriver, ComFprimeConfig::StackSizes::comDriver);
        }
        """

        phase Fpp.ToCpp.Phases.stopTasks """
        ComFprime::comDriver.stop();
        """

        phase Fpp.ToCpp.Phases.freeThreads """
        (void)ComFprime::comDriver.join();
        """
    }
}