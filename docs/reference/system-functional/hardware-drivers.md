
# Hardware Driver Functionality

## References

- [F Prime ByteStreamDriverModel SDD](https://github.com/nasa/fprime/blob/devel/Drv/ByteStreamDriverModel/docs/sdd.md)
- [F Prime ByteStreamBufferAdapter SDD](https://github.com/nasa/fprime/blob/devel/Drv/ByteStreamBufferAdapter/docs/sdd.md)
- [F Prime AsyncByteStreamBufferAdapter SDD](https://github.com/nasa/fprime/blob/devel/Drv/AsyncByteStreamBufferAdapter/docs/sdd.md)
- [F Prime Driver Interfaces SDD](https://github.com/nasa/fprime/blob/devel/Drv/Interfaces/docs/sdd.md)
- [F Prime TcpClient SDD](https://github.com/nasa/fprime/blob/devel/Drv/TcpClient/docs/sdd.md)
- [F Prime TcpServer SDD](https://github.com/nasa/fprime/blob/devel/Drv/TcpServer/docs/sdd.md)
- [F Prime Udp SDD](https://github.com/nasa/fprime/blob/devel/Drv/Udp/docs/sdd.md)
- [F Prime Ip SDD](https://github.com/nasa/fprime/blob/devel/Drv/Ip/docs/sdd.md)
- [F Prime LinuxUartDriver SDD](https://github.com/nasa/fprime/blob/devel/Drv/LinuxUartDriver/docs/sdd.md)
- [F Prime LinuxGpioDriver](https://github.com/nasa/fprime/blob/devel/Drv/LinuxGpioDriver)
- [F Prime LinuxI2cDriver](https://github.com/nasa/fprime/blob/devel/Drv/LinuxI2cDriver)
- [F Prime LinuxSpiDriver](https://github.com/nasa/fprime/blob/devel/Drv/LinuxSpiDriver)

## Overview

The driver layer provides hardware abstraction components that bridge the flight software's port-based communication model to physical hardware interfaces. Drivers implement standardized FPP interfaces so that higher-level components can interact with hardware through typed ports without knowledge of the underlying hardware specifics. The primary driver model is the byte stream interface, which is used for serial, network, and other stream-oriented communication. Additional interfaces cover GPIO, I2C, and SPI peripherals.

The F Prime core framework ships Linux-specific driver implementations. Other operating system and hardware platform support is provided by additional packages outside the core repository.

### Byte Stream Driver Model

The byte stream model defines a generic interface for drivers that operate with bidirectional byte streams. It provides:

- **Send** — An input port for transmitting data through the driver. The caller provides an Fw::Buffer, and the driver returns a status (success, retry, or error).
- **Receive** — An output port for delivering received data. The driver calls this port when data arrives, providing an Fw::Buffer containing the received bytes.

Two variants of the model exist:

- **Synchronous** — The send port blocks until the operation completes and returns status immediately.
- **Asynchronous** — The send port queues the request and returns status via a callback port.

### Byte Stream Buffer Adapters

Buffer adapters bridge the communication stack's buffer-based interface to the byte stream driver model. They handle buffer lifecycle management (allocation and deallocation) between the communication components and the underlying driver:

- **ByteStreamBufferAdapter** — Synchronous adapter for use with synchronous byte stream drivers.
- **AsyncByteStreamBufferAdapter** — Asynchronous adapter for use with async byte stream drivers.

### Network Drivers

F Prime provides network communication drivers that implement the byte stream model:

- **TCP Client** — Connects to a remote TCP server and provides bidirectional byte stream communication. Includes a receive thread for asynchronous data arrival. Supports automatic reconnection.
- **TCP Server** — Listens for incoming TCP connections. Operates similarly to the TCP client once a connection is established.
- **UDP** — Provides datagram-based communication. Supports both send and receive on UDP sockets.

These drivers share a common IP helper layer that provides socket management, address configuration, and connection handling.

### Serial Communication

The Linux UART Driver provides byte stream access to serial ports on Linux systems. It implements the byte stream driver model, allowing it to be used interchangeably with network drivers in the communication stack.

### GPIO

The GPIO interface provides read, write, and interrupt signaling for individual General Purpose Input/Output pins. The Linux GPIO Driver implements this interface using the Linux GPIO character device interface.

### I2C

The I2C interface provides read, write, and combined write-then-read operations for Inter-Integrated Circuit bus communication. The Linux I2C Driver implements this interface using the Linux I2C device interface.

### SPI

The SPI interface provides read and write operations for Serial Peripheral Interface bus communication. The Linux SPI Driver implements this interface using the Linux SPI device interface.

### Off Nominal

- Network drivers report connection failures and reconnect automatically when configured to do so.
- Byte stream send failures return appropriate status codes; the caller is responsible for retry or error handling.
- Hardware interface errors (I2C NAK, SPI transfer failure, UART framing error) are returned as typed status values through the driver interface.
