module Drv {
    interface Spi {
        @ Port to perform a synchronous write/read operation over the SPI bus
        guarded input port SpiWriteRead: Drv.SpiWriteRead

        @ TODO Mark SpiReadWrite as deprecated because it does not return a status, and indicate when it will be removed from FPrime
        @ Port to perform a synchronous read/write operation over the SPI bus
        sync input port SpiReadWrite: Drv.SpiReadWrite
    }
}
