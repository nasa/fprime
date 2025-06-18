module CommsCCSDSConfig {
    #Base ID for the CDHCore Subtopology, all components are offsets from this base ID
    constant BASE_ID = 0x8000
    
    module QueueSizes {
        constant comQueue    = 10
        constant cmdSeq      = 10
    }
    

    module StackSizes {
        constant comQueue   = 64 * 1024
        constant cmdSeq    = 64 * 1024
    }

    module Priorities {
        constant comQueue   = 101
        constant cmdSeq     = 100
    }
}

module CommsCCSDS {
    # Communications driver. May be swapped out with other comm drivers like UART in this file
    # to use another driver in the Comms Subtopology
    instance comDriver: Drv.TcpClient base id CommsCCSDSConfig.BASE_ID + 0x0B00 \ 
    {
        phase Fpp.ToCpp.Phases.configComponents """
        if (state.hostname != nullptr && state.port != 0) {
            CommsCCSDS::comDriver.configure(state.hostname, state.port);
        }
        """

        phase Fpp.ToCpp.Phases.startTasks """
        // Initialize socket client communication if and only if there is a valid specification
        if (state.hostname != nullptr && state.port != 0) {
            Os::TaskString name("ReceiveTask");
            CommsCCSDS::comDriver.start(name, 100, 100);
        }
        """

        phase Fpp.ToCpp.Phases.stopTasks """
        CommsCCSDS::comDriver.stop();
        """

        phase Fpp.ToCpp.Phases.freeThreads """
        (void)CommsCCSDS::comDriver.join();
        """
    }
}
