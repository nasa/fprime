module ComFprime {
    # ----------------------------------------------------------------------
    # Communication Driver Configuration
    # ----------------------------------------------------------------------
    # 
    # This file configures the communication driver for the ComFprime subtopology.
    # Two types of communication drivers are available:
    #
    # 1. TCP-based drivers (TcpClient/TcpServer) - for network communication
    # 2. UART-based drivers (LinuxUartDriver) - for serial communication
    #
    # Only ONE driver type should be active at a time. Comment out the driver
    # type you don't want to use.

    # ----------------------------------------------------------------------
    # TCP-based Communication Driver
    # ----------------------------------------------------------------------
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

    # ----------------------------------------------------------------------
    # UART-based Communication Driver
    # ----------------------------------------------------------------------
    # Uncomment the section below to use UART serial communication instead of TCP.
    # When enabling UART, make sure to comment out the TCP driver above to avoid conflicts.
    
    #instance comDriver: Drv.LinuxUartDriver base id ComFprimeConfig.BASE_ID + 0x0B00 \
    #{
    #    phase Fpp.ToCpp.Phases.startTasks"""
    #    if (state.comFprime.uartDevice != nullptr) {
    #        Os::TaskString name("ReceiveTask");
    #        // Open and start the UART communication driver
    #        if (ComFprime::comDriver.open(state.comFprime.uartDevice, static_cast<Drv::LinuxUartDriver::UartBaudRate>(state.comFprime.baudRate), 
    #            Drv::LinuxUartDriver::NO_FLOW, Drv::LinuxUartDriver::PARITY_NONE, 2048)) {
    #            ComFprime::comDriver.start(ComFprimeConfig::Priorities::comDriver, ComFprimeConfig::StackSizes::comDriver);
    #        } else {
    #            printf("Failed to open UART device %s at baud rate %" PRIu32 "\n", state.comFprime.uartDevice, state.comFprime.baudRate);
    #        }
    #    }
    #    """
    #
    #    phase Fpp.ToCpp.Phases.stopTasks """
    #    ComFprime::comDriver.quitReadThread();
    #    """
    #
    #    phase Fpp.ToCpp.Phases.freeThreads """
    #    (void)ComFprime::comDriver.join();
    #    """
    #}
}
