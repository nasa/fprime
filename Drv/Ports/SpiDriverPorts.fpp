module Drv {

  @ TODO: Update description: Serial Peripheral Interface Write a set of bytes then read a set of bytes
  @ TODO: Why is no chip select required?  Or SPI mode?  Is FPrime always presumed to be the slave?
  port SpiWriteRead(
                     ref writeBuffer: Fw.Buffer
                     ref readBuffer:  Fw.Buffer
                   ) -> Drv.SpiStatus

  @ TODO Mark SpiReadWrite as deprecated because it does not return a status, and indicate when it will be removed from FPrime
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

