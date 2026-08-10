module Drv {
    interface AsyncGuardedI2c {
        # ----------------------------------------------------------------------
        # I2C interface ports (async with callbacks)
        # ----------------------------------------------------------------------

        @ Port for asynchronous write transaction
        guarded input port write: [Drv.AsyncI2cCfg.I2cDriverPorts] Drv.I2cRequest

        @ Port for asynchronous read transaction
        guarded input port read: [Drv.AsyncI2cCfg.I2cDriverPorts] Drv.I2cRequest

        @ Port for asynchronous write-read transaction
        guarded input port writeRead: [Drv.AsyncI2cCfg.I2cDriverPorts] Drv.I2cWriteReadRequest

        ###### Ports below must be connected if buffers are being passed to/from i2c drv ######

        @ Port invoked when write transaction completes
        output port writeComplete: [Drv.AsyncI2cCfg.I2cDriverPorts] Drv.I2cCallback

        @ Port invoked when read transaction completes
        output port readComplete: [Drv.AsyncI2cCfg.I2cDriverPorts] Drv.I2cCallback

        @ Port invoked when write-read transaction completes
        output port writeReadComplete: [Drv.AsyncI2cCfg.I2cDriverPorts] Drv.I2cWriteReadCallback
    }
}
