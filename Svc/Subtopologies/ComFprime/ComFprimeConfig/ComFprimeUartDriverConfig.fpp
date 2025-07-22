module ComFprime {
    # ----------------------------------------------------------------------
    # UART-based Communication Driver Configuration
    # ----------------------------------------------------------------------
    # This file configures UART serial communication drivers for the ComFprime subtopology.
    
    instance comDriver: Drv.LinuxUartDriver base id ComFprimeConfig.BASE_ID + 0x0B00 \
    {
        phase Fpp.ToCpp.Phases.startTasks"""
        if (state.comFprime.uartDevice != nullptr) {
            Os::TaskString name("ReceiveTask");
            // Open and start the UART communication driver
            if (ComFprime::comDriver.open(state.comFprime.uartDevice, static_cast<Drv::LinuxUartDriver::UartBaudRate>(state.comFprime.baudRate), 
                Drv::LinuxUartDriver::NO_FLOW, Drv::LinuxUartDriver::PARITY_NONE, 2048)) {
                ComFprime::comDriver.start(ComFprimeConfig::Priorities::comDriver, ComFprimeConfig::StackSizes::comDriver);
            } else {
                printf("Failed to open UART device %s at baud rate %" PRIu32 "\n", state.comFprime.uartDevice, state.comFprime.baudRate);
            }
        }
        """

        phase Fpp.ToCpp.Phases.stopTasks """
        ComFprime::comDriver.quitReadThread();
        """

        phase Fpp.ToCpp.Phases.freeThreads """
        (void)ComFprime::comDriver.join();
        """
    }
} 