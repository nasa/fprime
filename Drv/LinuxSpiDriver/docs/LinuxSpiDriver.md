<title>LinuxSpiDriver Component Dictionary</title>
# LinuxSpiDriver Component Dictionary


## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|SPI_Bytes|0 (0x0)|U32|Bytes Sent/Received|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|SPI_OpenError|0 (0x0)|SPI open error| | | | |
| | | |device|I32||The device|
| | | |select|I32||The chip select|
| | | |error|I32||The error code|
|SPI_ConfigError|1 (0x1)|SPI config error| | | | |
| | | |device|I32||The device|
| | | |select|I32||The chip select|
| | | |error|I32||The error code|
|SPI_WriteError|2 (0x2)|SPI write error| | | | |
| | | |device|I32||The device|
| | | |select|I32||The chip select|
| | | |error|I32||The error code|
|SPI_PortOpened|4 (0x4)|SPI open notification| | | | |
| | | |device|I32||The device|
| | | |select|I32||The chip select|
