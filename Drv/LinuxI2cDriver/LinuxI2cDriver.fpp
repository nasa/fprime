module Drv {

  passive component LinuxI2cDriver {

    guarded input port write: Drv.I2c

    guarded input port read: Drv.I2c

    guarded input port writeRead: Drv.I2cWriteRead

  }

}
