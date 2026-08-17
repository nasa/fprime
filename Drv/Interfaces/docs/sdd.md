# Drv FPP Interfaces

The Drv interfaces are a set of `.fpp` files that define FPP interfaces for driver components to implement. An FPP interface is an FPP file that defines a set of ports. A component that implements an FPP interface must implement handlers for the input ports and has access to the output ports of the interface.

## Drv.ByteStreamDriver

The [`Drv.ByteStreamDriver`](../ByteStreamDriver.fpp) is an interface for writing and reading to a driver that acts as a byte stream, such as TCP/UDP/UART. The [`Drv::TcpClient`](../../TcpClient/TcpClient.fpp) driver implements this interface.

## Drv.AsyncByteStreamDriver

The [`Drv.AsyncByteStreamDriver`](../AsyncByteStreamDriver.fpp) is an interface for writing and reading to a byte stream driver where the send operation is non-blocking and status is returned through the `sendReturnOut` callback port.

## Drv.Gpio

The [`Drv.Gpio`](../Gpio.fpp) is an interface for writing and reading a single GPIO bit, and signaling a GPIO interrupt. The [`Drv::LinuxGpioDriver`](../../LinuxGpioDriver/LinuxGpioDriver.fpp) driver implements this interface.

## Drv.I2c

The [`Drv.I2c`](../I2c.fpp) is an interface for writing, reading, or writeRead to an I2C device. The [`Drv::LinuxI2cDriver`](../../LinuxI2cDriver/LinuxI2cDriver.fpp) driver implements this interface.

## Drv.AsyncI2c

The [`Drv.AsyncI2c`](../AsyncI2c.fpp) is an interface for asynchronous write, read, and write-read I2C transactions, with completion reported through callback ports.

## Drv.AsyncGuardedI2c

The [`Drv.AsyncGuardedI2c`](../AsyncGuardedI2c.fpp) is a variant of `Drv.AsyncI2c` whose transaction input ports are guarded rather than async.

## Drv.PassiveAsyncByteStreamDriverClient

The [`Drv.PassiveAsyncByteStreamDriverClient`](../PassiveAsyncByteStreamDriverClient.fpp) is an interface for a passive client of an asynchronous byte stream driver. It combines `Drv.PassiveByteStreamDriverClientReadyRecv` with an asynchronous send interface that receives buffer ownership back via a return port.

## Drv.PassiveBufferDriver

The [`Drv.PassiveBufferDriver`](../PassiveBufferDriver.fpp) is an interface for a passive buffer driver, composed of the `Fw.PassiveBufferIn` and `Fw.PassiveBufferOut` interfaces for sending data to and receiving data from the driver.

## Drv.PassiveBufferDriverClient

The [`Drv.PassiveBufferDriverClient`](../PassiveBufferDriverClient.fpp) is an interface for a passive client of a buffer driver, providing ports for sending buffers to the driver, receiving buffers from the driver, and returning buffer ownership in each direction.

## Drv.PassiveByteStreamDriverClient

The [`Drv.PassiveByteStreamDriverClient`](../PassiveByteStreamDriverClient.fpp) is an interface for a passive client of a synchronous byte stream driver, combining `Drv.PassiveByteStreamDriverClientReadyRecv` with a synchronous send port.

## Drv.PassiveByteStreamDriverClientReadyRecv

The [`Drv.PassiveByteStreamDriverClientReadyRecv`](../PassiveByteStreamDriverClientReadyRecv.fpp) is an interface providing the ready and receive ports for a byte stream driver client: a ready signal input, a data receive input, and a port for returning ownership of received buffers.

## Drv.Spi

The [`Drv.Spi`](../Spi.fpp) is an interface for writing and reading to an SPI device. The [`Drv::LinuxSpiDriver`](../../LinuxSpiDriver/LinuxSpiDriver.fpp) driver implements this interface.

## Drv.Tick

The [`Drv.Tick`](../Tick.fpp) is an interface for outputting a system tick along with a RawTime timestamp. The [`Svc::LinuxTimer`](../../../Svc/LinuxTimer/LinuxTimer.fpp) driver is an example of a Linux-based timer that implements this interface.
