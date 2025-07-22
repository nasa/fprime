module ComCcsds {
    # ----------------------------------------------------------------------
    # TCP-based Communication Driver Configuration
    # ----------------------------------------------------------------------
    # This file configures TCP communication drivers for the ComCcsds subtopology.
    # Choose between TcpClient (connects to a server) or TcpServer (accepts client connections)
    # To use TcpServer instead of TcpClient, simply swap the commented/uncommented lines below:
    
    #instance comDriver: Drv.TcpServer base id ComCcsdsConfig.BASE_ID + 0x0300 \
    instance comDriver: Drv.TcpClient base id ComCcsdsConfig.BASE_ID + 0x0300 \
    {
        phase Fpp.ToCpp.Phases.configComponents """
        if (state.comCcsds.hostname != nullptr && state.comCcsds.port != 0) {
            ComCcsds::comDriver.configure(state.comCcsds.hostname, state.comCcsds.port);
        }
        """

        phase Fpp.ToCpp.Phases.startTasks """
        // Initialize socket client communication if and only if there is a valid specification
        if (state.comCcsds.hostname != nullptr && state.comCcsds.port != 0) {
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