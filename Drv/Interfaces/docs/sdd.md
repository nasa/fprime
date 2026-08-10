# Drv FPP Interfaces

The Drv interfaces are a set of `.fppi` files that define FPP interfaces for driver components to implement. An FPP interface is an FPP file that defines a set of ports. A component that implements an FPP interface must implement handlers for the input ports and has access to the output ports of the interface.

## Drv/ByteStreamDriverInterface

The `Drv/ByteStreamDriverInterface` is an interface for writing and reading to a driver that acts as a byte stream, such as TCP/UDP/UART. The [`Drv::TcpClient`](../../TcpClient/TcpClient.fpp) driver implements this interface.

## Drv/GpioInterface

The `Drv/GpioInterface` is an interface for writing and reading a single GPIO bit, and signaling a GPIO interrupt. The [`Drv::LinuxGpioDriver`](../../LinuxGpioDriver/LinuxGpioDriver.fpp) driver implements this interface.

## Drv/I2cInterface

The `Drv/I2cInterface` is an interface for writing, reading, or writeRead to an I2C device. The [`Drv::LinuxI2cDriver`](../../LinuxI2cDriver/LinuxI2cDriver.fpp) driver implements this interface.

## Drv/SpiInterface

The `Drv/SpiInterface` is an interface for writing and reading to an SPI device. The [`Drv::LinuxSpiDriver`](../../LinuxSpiDriver/LinuxSpiDriver.fpp) driver implements this interface.

## Drv/TickInterface

The `Drv/TickInterface` is an interface for outputting a system tick along with a RawTime timestamp. The [`Svc::LinuxTimer`](../../../Svc/LinuxTimer/LinuxTimer.fpp) driver is an example of a Linux-based timer that implements this interface.

<!-- fpp-dictionary-begin -->
## Interface Dictionary

The following tables are derived from the interface FPP models in this directory.

### Port Descriptions

| Name | Kind | Port Type | Description |
|---|---|---|---|
| `ready` | `output` | `Drv.ByteStreamReady` | Port invoked when the driver is ready to send/receive data |
| `recv` | `output` | `Drv.ByteStreamData` | Port invoked by the driver when it receives data |
| `send` | `async input` | `Fw.BufferSend` | Invoke this port to send data out the driver (asynchronous) Status and ownership of the buffer are returned through the sendReturnOut callback |
| `sendReturnOut` | `output` | `Drv.ByteStreamData` | Port returning ownership of data received on $send port |
| `recvReturnIn` | `guarded input` | `Fw.BufferSend` | Port receiving back ownership of data sent out on $recv port |
| `write` | `guarded input` | `[Drv.AsyncI2cCfg.I2cDriverPorts] Drv.I2cRequest` | Port for asynchronous write transaction |
| `read` | `guarded input` | `[Drv.AsyncI2cCfg.I2cDriverPorts] Drv.I2cRequest` | Port for asynchronous read transaction |
| `writeRead` | `guarded input` | `[Drv.AsyncI2cCfg.I2cDriverPorts] Drv.I2cWriteReadRequest` | Port for asynchronous write-read transaction |
| `writeComplete` | `output` | `[Drv.AsyncI2cCfg.I2cDriverPorts] Drv.I2cCallback` | Port invoked when write transaction completes |
| `readComplete` | `output` | `[Drv.AsyncI2cCfg.I2cDriverPorts] Drv.I2cCallback` | Port invoked when read transaction completes |
| `writeReadComplete` | `output` | `[Drv.AsyncI2cCfg.I2cDriverPorts] Drv.I2cWriteReadCallback` | Port invoked when write-read transaction completes |
| `write` | `async input` | `[Drv.AsyncI2cCfg.I2cDriverPorts] Drv.I2cRequest` | Port for asynchronous write transaction |
| `read` | `async input` | `[Drv.AsyncI2cCfg.I2cDriverPorts] Drv.I2cRequest` | Port for asynchronous read transaction |
| `writeRead` | `async input` | `[Drv.AsyncI2cCfg.I2cDriverPorts] Drv.I2cWriteReadRequest` | Port for asynchronous write-read transaction |
| `writeComplete` | `output` | `[Drv.AsyncI2cCfg.I2cDriverPorts] Drv.I2cCallback` | Port invoked when write transaction completes |
| `readComplete` | `output` | `[Drv.AsyncI2cCfg.I2cDriverPorts] Drv.I2cCallback` | Port invoked when read transaction completes |
| `writeReadComplete` | `output` | `[Drv.AsyncI2cCfg.I2cDriverPorts] Drv.I2cWriteReadCallback` | Port invoked when write-read transaction completes |
| `ready` | `output` | `Drv.ByteStreamReady` | Port invoked when the driver is ready to send/receive data |
| `recv` | `output` | `Drv.ByteStreamData` | Port invoked by the driver when it receives data |
| `send` | `guarded input` | `Drv.ByteStreamSend` | Invoke this port to send data out the driver (synchronous) Status is returned, and ownership of the buffer is retained by the caller |
| `recvReturnIn` | `guarded input` | `Fw.BufferSend` | Port receiving back ownership of data sent out on $recv port |
| `drvConnected` | `sync input` | `Drv.ByteStreamReady` | Ready signal when driver is connected |
| `drvReceiveIn` | `sync input` | `Drv.ByteStreamData` | Receive (read) data from driver. |
| `drvReceiveReturnOut` | `output` | `Fw.BufferSend` | Returning ownership of buffer that came in on drvReceiveIn |
| `drvSendOut` | `output` | `Drv.ByteStreamSend` | Send (write) data to the driver |
| `gpioWrite` | `sync input` | `Drv.GpioWrite` | Port used to write to a GPIO pin |
| `gpioRead` | `sync input` | `Drv.GpioRead` | Port used to read from a GPIO pin |
| `gpioInterrupt` | `output` | `Svc.Cycle` | Port used to indicate transition on the GPIO pin |
| `write` | `guarded input` | `Drv.I2c` | Port for guarded synchronous writing to I2C |
| `read` | `guarded input` | `Drv.I2c` | Port for guarded synchronous reading from I2C |
| `writeRead` | `guarded input` | `Drv.I2cWriteRead` | Port for synchronous writing and reading from I2C |
| `toByteStreamDriver` | `output` | `Fw.BufferSend` | Port for sending data to the driver Sample connection: client.toByteStreamDriver -> driver.$send |
| `toByteStreamDriverReturn` | `sync input` | `Drv.ByteStreamData` | Port for receiving buffers sent on toByteStreamDriver and then returned Sample connection: driver.sendReturnOut -> client.toByteStreamDriverReturn |
| `toBufferDriver` | `output` | `Fw.BufferSend` | Port for sending data to the driver Sample connection: client.toBufferDriver -> driver.bufferIn |
| `toBufferDriverReturn` | `sync input` | `Fw.BufferSend` | Port for receiving buffers sent on toBufferDriver and then returned Sample connection: driver.bufferInReturn -> client.toBufferDriverReturn |
| `fromBufferDriver` | `sync input` | `Fw.BufferSend` | Port for receiving data from the driver Sample connection: driver.bufferOut -> client.fromBufferDriver |
| `fromBufferDriverReturn` | `output` | `Fw.BufferSend` | Port for returning buffers received on fromBufferDriver Sample connection: client.fromBufferDriverReturn -> driver.bufferOutReturn |
| `toByteStreamDriver` | `output` | `Drv.ByteStreamSend` | Port for sending data to the driver Sample connection: client.toByteStreamDriver -> driver.$send |
| `byteStreamDriverReady` | `sync input` | `Drv.ByteStreamReady` | Port for receiving ready signals from the driver Sample connection: byteStreamDriver.ready -> byteStreamDriverClient.byteStreamDriverReady |
| `fromByteStreamDriver` | `sync input` | `Drv.ByteStreamData` | Port for receiving data from the driver Sample connection: byteStreamDriver.$recv -> byteStreamDriverClient.fromDriver |
| `fromByteStreamDriverReturn` | `output` | `Fw.BufferSend` | Port for returning ownership of buffers received on fromDriver Sample connection: byteStreamDriverClient.byteStreamReturn -> byteStreamDriver.recvReturnIn |
| `SpiWriteRead` | `guarded input` | `Drv.SpiWriteRead` | Port to perform a synchronous write/read operation over the SPI bus |
| `SpiReadWrite` | `sync input` | `Drv.SpiReadWrite` | DEPRECATED Use SpiWriteRead port instead (same operation with a return value) Port to perform a synchronous read/write operation over the SPI bus |
| `CycleOut` | `output` | `Svc.Cycle` | The cycle outputs. Meant to be connected to rate group driver |

<!-- fpp-dictionary-end -->
