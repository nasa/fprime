module Drv {
    interface AsyncSpi {
        # ----------------------------------------------------------------------
        # SPI interface ports (async with callbacks)
        # Port index = device slot (chip select); the completion is sent on the
        # same index the request arrived on.
        # ----------------------------------------------------------------------

        @ Port for asynchronous write/read transaction (queued; no acceptance status, see callback)
        async input port SpiWriteRead: [Drv.AsyncSpiCfg.SpiDriverPorts] Drv.SpiWriteReadQueuedRequest

        ###### Port below must be connected: every request is completed through it ######

        @ Port invoked when the write/read transaction completes
        output port SpiWriteReadComplete: [Drv.AsyncSpiCfg.SpiDriverPorts] Drv.SpiWriteReadCallback
    }
}
