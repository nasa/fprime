module ComFprime {
    # ----------------------------------------------------------------------
    # TCP-based Communication Driver Configuration
    # ----------------------------------------------------------------------
    # This file configures TCP communication drivers for the ComFprime subtopology.
    # Choose between TcpClient (connects to a server) or TcpServer (accepts client connections)
    # To use TcpServer instead of TcpClient, simply swap the commented/uncommented lines below:
    
    #instance comDriver: Drv.TcpServer base id ComFprimeConfig.BASE_ID + 0x0B00 \
    instance comDriver: Drv.TcpClient base id ComFprimeConfig.BASE_ID + 0x0B00 \
    {
        phase Fpp.ToCpp.Phases.configComponents """
        if (state.comFprime.hostname != nullptr && state.comFprime.port != 0) {
            ComFprime::comDriver.configure(state.comFprime.hostname, state.comFprime.port);
        }
        """

        phase Fpp.ToCpp.Phases.startTasks """
        // Initialize socket client communication if and only if there is a valid specification
        if (state.comFprime.hostname != nullptr && state.comFprime.port != 0) {
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