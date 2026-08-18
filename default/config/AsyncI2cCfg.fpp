module Drv {
    module AsyncI2cCfg {
        @ Number of async I2c driver ports for AsyncI2c, AsyncGuardedI2c, and AsyncSyncI2c interfaces.
        @ Each component using the I2C driver should reserve a port number for each send/response ports.
        constant I2cDriverPorts = 10
    }
}
