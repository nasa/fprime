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
