module Drv {

  @ Write a set of bytes then read a set of bytes using the repeated start option
  port I2cWriteRead(
                     addr: U32 @< I2C slave device address
                     ref writeBuffer: Fw.Buffer @< Buffer to write data to the i2c device
                     ref readBuffer: Fw.Buffer @< Buffer to read back data from the i2c device, must set size when passing in read buffer
                   ) -> Drv.I2cStatus

}

module Drv {

  enum I2cStatus : U8 {
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

module Drv {
  @ I2C write/read request port (async, no return value)
    port I2cRequest(
        addr: U32 @< I2C slave device address
        ref buffer: Fw.Buffer @< Buffer with data to write or space for read data
    )

    @ I2C write-read request port (async, no return value)
    port I2cWriteReadRequest(
        addr: U32 @< I2C slave device address
        ref writeBuffer: Fw.Buffer @< Buffer to write
        ref readBuffer: Fw.Buffer @< Buffer to read into
    )

    @ I2C write/read completion callback port
    port I2cCallback(
        ref buffer: Fw.Buffer @< Buffer used in transaction (contains read data for read operations)
        status: Drv.I2cStatus @< Transaction result status
    )

    @ I2C write-read completion callback port
    port I2cWriteReadCallback(
        ref writeBuffer: Fw.Buffer @< Write buffer used in transaction
        ref readBuffer: Fw.Buffer @< Read buffer with received data
        status: Drv.I2cStatus @< Transaction result status
    )
}