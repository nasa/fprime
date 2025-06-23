module ComCcsdsConfig {
    #Base ID for the ComCcsds Subtopology, all components are offsets from this base ID
    constant BASE_ID = 0x8000
    
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
        constant events      = 100             
        constant tlm         = 500            
        constant file        = 100            
    }

    module QueuePriorities {
        constant events      = 0                 
        constant tlm         = 2                 
        constant file        = 1                   
    }

    # Buffer management constants
    module BufferMgr {
        constant cmdSeqBuffer           = 5 * 1024  # 5KB for command sequencer buffer
        constant frameAccumulator       = 2048      # 2KB frame accumulator buffer
        constant commsBufferStore       = 2048      # 2KB communications buffer store
        constant commsFileBufferStore   = 3000      # 3KB file buffer store
        constant commsBufferCount       = 20        # Number of comms buffers
        constant commsFileBufferCount   = 30        # File queue buffer count
        constant commsBufferManager     = 200       # Buffer manager identifier
    }
}

module ComCcsds {
    # Communications driver. May be swapped out with other comm drivers like UART in this file
    # to use another driver in the Comms Subtopology
    instance comDriver: Drv.TcpClient base id ComCcsdsConfig.BASE_ID + 0x0B00 \ 
    {
        phase Fpp.ToCpp.Phases.configComponents """
        if (state.hostname != nullptr && state.port != 0) {
            ComCcsds::comDriver.configure(state.hostname, state.port);
        }
        """

        phase Fpp.ToCpp.Phases.startTasks """
        // Initialize socket client communication if and only if there is a valid specification
        if (state.hostname != nullptr && state.port != 0) {
            Os::TaskString name("ReceiveTask");
            ComCcsds::comDriver.start(name, ComCcsdsConfig::Priorities::comDriver, ComCcsdsConfig::StackSizes::comDriver);
        }
        """

        phase Fpp.ToCpp.Phases.stopTasks """
        ComCcsds::comDriver.stop();
        """

        phase Fpp.ToCpp.Phases.freeThreads """
        (void)ComCcsds::comDriver.join();
        """
    }
}
