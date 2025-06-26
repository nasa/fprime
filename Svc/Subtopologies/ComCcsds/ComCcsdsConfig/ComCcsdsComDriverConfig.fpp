module ComCcsds {
    # Communications driver. May be swapped out with other comm drivers like UART in this file
    # to use another driver in the Comms Subtopology
    instance comDriver: Drv.TcpClient base id ComCcsdsConfig.BASE_ID + 0x0B00 \ 
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
