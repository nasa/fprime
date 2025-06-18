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
    }

    module Priorities {
        constant comQueue   = 101
        constant cmdSeq     = 100
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
            ComFprime::comDriver.start(name, 100, 100);
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