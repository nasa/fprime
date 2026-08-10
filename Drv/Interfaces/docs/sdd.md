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
