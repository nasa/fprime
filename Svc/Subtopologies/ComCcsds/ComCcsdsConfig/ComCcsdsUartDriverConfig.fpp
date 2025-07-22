module ComCcsds {
    # ----------------------------------------------------------------------
    # UART-based Communication Driver Configuration
    # ----------------------------------------------------------------------
    # This file configures UART serial communication drivers for the ComCcsds subtopology.
    
    instance comDriver: Drv.LinuxUartDriver base id ComCcsdsConfig.BASE_ID + 0x0300 \
    {
        phase Fpp.ToCpp.Phases.startTasks"""
        if (state.comCcsds.uartDevice != nullptr) {
            Os::TaskString name("ReceiveTask");
            // Open and start the UART communication driver
            if (ComCcsds::comDriver.open(state.comCcsds.uartDevice, static_cast<Drv::LinuxUartDriver::UartBaudRate>(state.comCcsds.baudRate), 
                Drv::LinuxUartDriver::NO_FLOW, Drv::LinuxUartDriver::PARITY_NONE, 2048)) {
                ComCcsds::comDriver.start(ComCcsdsConfig::Priorities::comDriver, ComCcsdsConfig::StackSizes::comDriver);
            } else {
                printf("Failed to open UART device %s at baud rate %" PRIu32 "\n", state.comCcsds.uartDevice, state.comCcsds.baudRate);
            }
        }
        """

        phase Fpp.ToCpp.Phases.stopTasks """
        ComCcsds::comDriver.quitReadThread();
        """

        phase Fpp.ToCpp.Phases.freeThreads """
        (void)ComCcsds::comDriver.join();
        """
    }
} 