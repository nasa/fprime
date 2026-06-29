module Drv {
    interface AsyncGuardedI2c {
        # ----------------------------------------------------------------------
        # I2C interface ports (async with callbacks)
        # ----------------------------------------------------------------------

        @ Port for asynchronous write transaction
        guarded input port write: Drv.I2cRequest

        @ Port for asynchronous read transaction
        guarded input port read: Drv.I2cRequest

        @ Port for asynchronous write-read transaction
        guarded input port writeRead: Drv.I2cWriteReadRequest

        ###### Ports below must be connected if buffers are being passed to/from i2c drv ######

        @ Port invoked when write transaction completes
        output port writeComplete: Drv.I2cCallback

        @ Port invoked when read transaction completes
        output port readComplete: Drv.I2cCallback

        @ Port invoked when write-read transaction completes
        output port writeReadComplete: Drv.I2cWriteReadCallback
    }
}
