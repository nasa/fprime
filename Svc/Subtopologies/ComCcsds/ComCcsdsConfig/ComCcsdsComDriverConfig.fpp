module ComCcsds {
    # ----------------------------------------------------------------------
    # Communication Driver Configuration
    # ----------------------------------------------------------------------
    # 
    # This file configures the communication driver for the ComCcsds subtopology.
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
    
    #instance comDriver: Drv.TcpServer base id ComCcsdsConfig.BASE_ID + 0x0B00 \
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

    # ----------------------------------------------------------------------
    # UART-based Communication Driver
    # ----------------------------------------------------------------------
    # Uncomment the section below to use UART serial communication instead of TCP.
    # When enabling UART, make sure to comment out the TCP driver above to avoid conflicts.
    
    #instance comDriver: Drv.LinuxUartDriver base id ComCcsdsConfig.BASE_ID + 0x0B00 \
    #{
    #    phase Fpp.ToCpp.Phases.startTasks"""
    #    if (state.comCcsds.uartDevice != nullptr) {
    #        Os::TaskString name("ReceiveTask");
    #        // Open and start the UART communication driver
    #        if (ComCcsds::comDriver.open(state.comCcsds.uartDevice, static_cast<Drv::LinuxUartDriver::UartBaudRate>(state.comCcsds.baudRate), 
    #            Drv::LinuxUartDriver::NO_FLOW, Drv::LinuxUartDriver::PARITY_NONE, 2048)) {
    #            ComCcsds::comDriver.start(ComCcsdsConfig::Priorities::comDriver, ComCcsdsConfig::StackSizes::comDriver);
    #        } else {
    #            printf("Failed to open UART device %s at baud rate %" PRIu32 "\n", state.comCcsds.uartDevice, state.comCcsds.baudRate);
    #        }
    #    }
    #    """
    #
    #    phase Fpp.ToCpp.Phases.stopTasks """
    #    ComCcsds::comDriver.quitReadThread();
    #    """
    #
    #    phase Fpp.ToCpp.Phases.freeThreads """
    #    (void)ComCcsds::comDriver.join();
    #    """
    #}
}
