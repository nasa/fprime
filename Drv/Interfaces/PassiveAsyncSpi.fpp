module Drv {
    @ Asynchronous SPI interface for passive drivers (e.g. DMA- or interrupt-driven baremetal drivers)
    interface PassiveAsyncSpi {
        # ----------------------------------------------------------------------
        # SPI interface ports (async with callbacks)
        # Port index = device slot (chip select); the completion is sent on the
        # same index the request arrived on.
        # ----------------------------------------------------------------------

        @ Port for asynchronous write/read transaction (returns the acceptance status)
        sync input port SpiWriteRead: [Drv.AsyncSpiCfg.SpiDriverPorts] Drv.SpiWriteReadRequest

        ###### Port below must be connected: every accepted request is completed through it ######

        @ Port invoked when the write/read transaction completes
        output port SpiWriteReadComplete: [Drv.AsyncSpiCfg.SpiDriverPorts] Drv.SpiWriteReadCallback
    }
}
