<title>LinuxGpioDriver Component Dictionary</title>
# LinuxGpioDriver Component Dictionary



## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|GP_OpenError|0 (0x0)|Open error| | | | |
| | | |gpio|I32||The device|
| | | |error|I32||The error code|
| | | |msg|Fw::LogStringArg&|40|The error string|
|GP_ConfigError|1 (0x1)|GPIO configure error| | | | |
| | | |gpio|I32||The device|
| | | |error|I32||The error code|
|GP_WriteError|2 (0x2)|GPIO write error| | | | |
| | | |gpio|I32||The device|
| | | |error|I32||The error code|
|GP_ReadError|3 (0x3)|GPIO read error| | | | |
| | | |gpio|I32||The device|
| | | |error|I32||The error code|
|GP_PortOpened|4 (0x4)|GPIO opened notification| | | | |
| | | |gpio|I32||The device|
|GP_IntStartError|5 (0x5)|GPIO interrupt start error notification| | | | |
| | | |gpio|I32||The device|
|GP_IntWaitError|6 (0x6)|GPIO interrupt wait error notification| | | | |
| | | |gpio|I32||The device|
