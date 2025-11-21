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
  }

}

