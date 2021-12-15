@ SPI open error
event SPI_OpenError(
                     device: I32 @< The device
                     select: I32 @< The chip select
                     error: I32 @< The error code
                   ) \
  severity warning high \
  id 0 \
  format "Error opening SPI device {}.{}: {}"

@ SPI config error
event SPI_ConfigError(
                       device: I32 @< The device
                       select: I32 @< The chip select
                       error: I32 @< The error code
                     ) \
  severity warning high \
  id 1 \
  format "Error configuring SPI device {}.{}: {}"

@ SPI write error
event SPI_WriteError(
                      device: I32 @< The device
                      select: I32 @< The chip select
                      error: I32 @< The error code
                    ) \
  severity warning high \
  id 2 \
  format "Error writing/reading SPI device {}.{}: {}" \
  throttle 5

@ SPI open notification
event SPI_PortOpened(
                      device: I32 @< The device
                      select: I32 @< The chip select
                    ) \
  severity activity high \
  id 4 \
  format "SPI Device {}.{} configured"
