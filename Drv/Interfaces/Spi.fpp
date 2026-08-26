module Drv {
    interface Spi {
        @ Port to perform a synchronous write/read operation over the SPI bus
        guarded input port SpiWriteRead: Drv.SpiWriteRead

        @ DEPRECATED Use SpiWriteRead port instead (same operation with a return value)
        @ Port to perform a synchronous read/write operation over the SPI bus
        sync input port SpiReadWrite: Drv.SpiReadWrite
    }
}
