module Drv {

  port SpiWriteRead(
                     ref writeBuffer: Fw.Buffer
                     ref readBuffer:  Fw.Buffer
                   ) -> Drv.SpiStatus

  @ DEPRECATED: Use SpiWriteRead port instead (same operation with a return value)
  port SpiReadWrite(
                     ref writeBuffer: Fw.Buffer
                     ref readBuffer:  Fw.Buffer
                   )

}

module Drv {

  enum SpiStatus : U8 {
    SPI_OK           = 0 @< Transaction okay
    SPI_OPEN_ERR     = 1 @< SPI driver failed to open device
    SPI_CONFIG_ERR   = 2 @< SPI read failed
    SPI_MISMATCH_ERR = 3 @< SPI read failed
    SPI_WRITE_ERR    = 4 @< SPI write failed
    SPI_OTHER_ERR    = 5 @< Other errors that do not fit
    SPI_BUSY_ERR     = 6 @< SPI driver busy with another transaction (asynchronous interfaces)
  }

}

module Drv {

  @ Request an asynchronous SPI write/read transaction (no return value, like Drv.I2cRequest)
  @ Exactly one Drv.SpiWriteReadCallback invocation follows every request, carrying the
  @ outcome in its status; a request the driver cannot take (e.g. SPI_BUSY_ERR while another
  @ transaction is in flight, SPI_CONFIG_ERR before configuration) is reported the same way.
  port SpiWriteReadRequest(
                            ref writeBuffer: Fw.Buffer @< Buffer with data to write
                            ref readBuffer:  Fw.Buffer @< Buffer to read into, size set by the caller
                          )

  @ Completion callback for an asynchronous SPI write/read transaction
  @ Both buffers are handed back with the data pointer and size they had in the request.
  @ Drivers must invoke this port from a non-interrupt context.
  port SpiWriteReadCallback(
                             ref writeBuffer: Fw.Buffer @< Write buffer passed in the request
                             ref readBuffer:  Fw.Buffer @< Read buffer passed in the request, holding received data
                             status: Drv.SpiStatus @< Transaction result status
                           )

}
