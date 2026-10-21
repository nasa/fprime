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

  @ Request an asynchronous SPI write/read transaction (guarded and sync variants)
  @ The return value is the acceptance status: SPI_OK means the transfer was started and
  @ exactly one Drv.SpiWriteReadCallback invocation will follow for these buffers. Any other
  @ status means the request was refused synchronously (e.g. SPI_BUSY_ERR, SPI_CONFIG_ERR)
  @ and no callback follows; the caller keeps ownership of both buffers.
  port SpiWriteReadRequest(
                            ref writeBuffer: Fw.Buffer @< Buffer with data to write
                            ref readBuffer:  Fw.Buffer @< Buffer to read into, size set by the caller
                          ) -> Drv.SpiStatus

  @ Request an asynchronous SPI write/read transaction (queued variant, no return value)
  @ Exactly one Drv.SpiWriteReadCallback invocation follows every request; a refused
  @ request is reported through the callback status.
  port SpiWriteReadQueuedRequest(
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
