module Drv {
    module AsyncSpiCfg {
        @ Number of asynchronous SPI driver ports for the AsyncSpi and AsyncGuardedSpi interfaces.
        @ Each client of the SPI driver should reserve one port index for its request/completion port pair.
        constant SpiDriverPorts = 10
    }
}
