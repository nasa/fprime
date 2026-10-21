# Drv FPP Interfaces

The Drv interfaces are a set of `.fpp` files that define FPP interfaces for driver components to implement. An FPP interface is an FPP file that defines a set of ports. A component that implements an FPP interface must implement handlers for the input ports and has access to the output ports of the interface.

## Drv.ByteStreamDriver

The [`Drv.ByteStreamDriver`](../ByteStreamDriver.fpp) is an interface for writing and reading to a driver that acts as a byte stream, such as TCP/UDP/UART. The [`Drv::TcpClient`](../../TcpClient/TcpClient.fpp) driver implements this interface.

## Drv.Gpio

The [`Drv.Gpio`](../Gpio.fpp) is an interface for writing and reading a single GPIO bit, and signaling a GPIO interrupt. The [`Drv::LinuxGpioDriver`](../../LinuxGpioDriver/LinuxGpioDriver.fpp) driver implements this interface.

## Drv.I2c

The [`Drv.I2c`](../I2c.fpp) is an interface for writing, reading, or writeRead to an I2C device. The [`Drv::LinuxI2cDriver`](../../LinuxI2cDriver/LinuxI2cDriver.fpp) driver implements this interface.

## Drv.Spi

The [`Drv.Spi`](../Spi.fpp) is an interface for writing and reading to an SPI device. The [`Drv::LinuxSpiDriver`](../../LinuxSpiDriver/LinuxSpiDriver.fpp) driver implements this interface.

## Drv.AsyncSpi, Drv.AsyncGuardedSpi, Drv.PassiveAsyncSpi

The [`Drv.AsyncSpi`](../AsyncSpi.fpp), [`Drv.AsyncGuardedSpi`](../AsyncGuardedSpi.fpp), and [`Drv.PassiveAsyncSpi`](../PassiveAsyncSpi.fpp) interfaces are the asynchronous counterparts of `Drv.Spi`, following the pattern of `Drv.AsyncI2c` and `Drv.AsyncGuardedI2c`. A client requests a write/read transaction on the `SpiWriteRead` input port and continues; the driver performs the transfer (typically by DMA or interrupt) and reports completion on the `SpiWriteReadComplete` output port, handing both buffers back with the data pointer and size they had in the request. No driver in the core framework implements these interfaces; they are intended for platform packages (e.g. DMA-driven baremetal SPI drivers), which previously had to carry a private copy of this contract.

The three variants differ only in the kind of the request port, so that a driver can pick the one matching its execution model:

| Interface | `SpiWriteRead` port kind | Request port type | Implementing component |
|---|---|---|---|
| `Drv.AsyncSpi` | `async input` | `Drv.SpiWriteReadQueuedRequest` (no return value) | queued or active |
| `Drv.AsyncGuardedSpi` | `guarded input` | `Drv.SpiWriteReadRequest` (returns `Drv.SpiStatus`) | any |
| `Drv.PassiveAsyncSpi` | `sync input` | `Drv.SpiWriteReadRequest` (returns `Drv.SpiStatus`) | passive (no mutex, e.g. no-RTOS baremetal) |

**Port indexing.** Both ports are arrays of size `Drv.AsyncSpiCfg.SpiDriverPorts` (default 10, configurable in `config/AsyncSpiCfg.fpp`). The index is the device slot, i.e. the chip select the transaction targets, and the completion for a request received on index `n` is always sent on `SpiWriteReadComplete[n]`. Each client reserves one index and connects its request output and its completion input to that same index.

**Acceptance status (guarded and sync variants).** `Drv.SpiWriteReadRequest` returns the *acceptance* status of the request, not the outcome of the transfer. `SPI_OK` means the transfer was started and exactly one `SpiWriteReadComplete` call will follow for these buffers. Any other status (e.g. `SPI_BUSY_ERR` while a transfer is in flight, `SPI_CONFIG_ERR` before the driver is configured) means the request was refused synchronously, no completion call follows, and the caller keeps ownership of both buffers. The synchronous return lets a passive client learn of a refusal inside its own call rather than through a re-entrant completion callback.

**Queued variant.** FPP does not allow an `async input` port to return a value, so `Drv.AsyncSpi` uses `Drv.SpiWriteReadQueuedRequest` instead. Because the request is dispatched later on the driver's own thread, a refusal cannot be reported synchronously: the driver must complete *every* request through `SpiWriteReadComplete`, reporting a refusal through the callback `status`.

**Completion context.** `Drv.SpiWriteReadCallback` carries the outcome of the transfer in `status` (`SPI_OK` on success). An implementing driver must invoke `SpiWriteReadComplete` from a non-interrupt context: it is the driver's responsibility to leave ISR context (e.g. by deferring to a scheduled or queued handler) before calling downstream components.

## Drv.Tick

The [`Drv.Tick`](../Tick.fpp) is an interface for outputting a system tick along with a RawTime timestamp. The [`Svc::LinuxTimer`](../../../Svc/LinuxTimer/LinuxTimer.fpp) driver is an example of a Linux-based timer that implements this interface.
