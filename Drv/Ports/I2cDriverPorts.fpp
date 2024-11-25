module Drv {

  @ Write a set of bytes then read a set of bytes using the repeated start option
  port I2cWriteRead(
                     addr: U32 @< I2C slave device address
                     ref writeBuffer: Fw.Buffer @< Buffer to write data to the i2c device
                     ref readBuffer: Fw.Buffer @< Buffer to read back data from the i2c device, must set size when passing in read buffer
                   ) -> Drv.I2cStatus

}

module Drv {

  enum I2cStatus {
    I2C_OK = 0 @< Transaction okay
    I2C_ADDRESS_ERR = 1 @< I2C address invalid
    I2C_WRITE_ERR = 2 @< I2C write failed
    I2C_READ_ERR = 3 @< I2C read failed
    I2C_OPEN_ERR = 4 @< I2C driver failed to open device
    I2C_OTHER_ERR = 5 @< Other errors that don't fit
  }

}

module Drv {

  port I2c(
            addr: U32 @< I2C slave device address
            ref serBuffer: Fw.Buffer @< Buffer with data to read/write to/from
          ) -> Drv.I2cStatus

}
