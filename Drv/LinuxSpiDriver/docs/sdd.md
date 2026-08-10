# Drv::LinuxSpiDriver

## 1. Introduction

The LinuxSpiDriver component provides a Linux-specific implementation of an SPI (Serial Peripheral Interface) bus master driver. It implements the [`Drv.Spi`](../../Interfaces/Spi.fpp) interface, exposing synchronous ports for performing full-duplex write/read transactions with SPI slave devices.

The component wraps the Linux userspace `spidev` interface (`/dev/spidev<device>.<select>`, via `ioctl` with `SPI_IOC_MESSAGE`) to perform bus transactions on behalf of client components.

## 2. Requirements

| Name | Description | Validation |
|---|---|---|
| LINUX-SPI-COMP-001 | The LinuxSpiDriver component shall implement the Drv.Spi interface | inspection |
| LINUX-SPI-COMP-002 | The LinuxSpiDriver component shall support opening a Linux spidev device by bus number and chip select | inspection |
| LINUX-SPI-COMP-003 | The LinuxSpiDriver component shall support configurable clock frequencies (1, 5, 10, 15, and 20 MHz) | inspection |
| LINUX-SPI-COMP-004 | The LinuxSpiDriver component shall support the four SPI clock polarity/phase modes | inspection |
| LINUX-SPI-COMP-005 | The LinuxSpiDriver component shall perform synchronous full-duplex write/read transactions | inspection |
| LINUX-SPI-COMP-006 | The LinuxSpiDriver component shall report open, configuration, and transfer errors via events | inspection |
| LINUX-SPI-COMP-007 | The LinuxSpiDriver component shall report the cumulative number of bytes transferred via telemetry | inspection |

## 3. Design

The LinuxSpiDriver is a passive component. All operations execute synchronously on the caller's thread. The primary `SpiWriteRead` port is guarded, so concurrent transactions from multiple callers are serialized by the component's mutex.

### 3.1 Port Description

| Port | Kind | Type | Description |
|---|---|---|---|
| `SpiWriteRead` | guarded input | `Drv.SpiWriteRead` | Full-duplex write/read transaction; returns `SpiStatus` |
| `SpiReadWrite` | sync input | `Drv.SpiReadWrite` | DEPRECATED: same operation without a return value; use `SpiWriteRead` instead |

### 3.2 Events

| Event | Severity | Description |
|---|---|---|
| `SPI_OpenError` | warning high | Error opening the spidev device |
| `SPI_ConfigError` | warning high | Error configuring mode, bits-per-word, or clock frequency |
| `SPI_WriteError` | warning high | Error performing a bus transfer (throttled after 5 occurrences) |
| `SPI_ConfigMismatch` | warning low | Read-back of a configured parameter did not match the written value |
| `SPI_PortOpened` | activity high | Device successfully opened and configured |

### 3.3 Telemetry

| Channel | Type | Description |
|---|---|---|
| `SPI_Bytes` | FwSizeType | Cumulative bytes sent/received |

### 3.4 Transaction Handling

1. The `open()` method opens `/dev/spidev<device>.<select>`, then configures the SPI mode (clock polarity/phase per the `SpiMode` enumeration), bits per word, and clock frequency (per the `SpiFrequency` enumeration) via `ioctl`. Each written setting is read back and any mismatch is reported with `SPI_ConfigMismatch`.
2. The `SpiWriteRead` handler performs a full-duplex transfer with `ioctl(SPI_IOC_MESSAGE)`: the write buffer is shifted out while the read buffer is filled, both sized by the provided `Fw::Buffer` objects. On success the byte counter is updated and `SpiStatus::SPI_OK` is returned; on failure an `SPI_WriteError` event is emitted and an error status is returned.
3. The deprecated `SpiReadWrite` handler performs the same operation but discards the status.

### 3.5 Stub Implementation

A stub implementation (`LinuxSpiDriverComponentImplStub.cpp`) is provided for platforms without Linux spidev support. The build selects the real or stub implementation based on the target platform.

## 4. Usage

The typical usage pattern is:

1. **Instantiate** the component in the topology.
2. **Open the device** by calling `open(device, select, clock, spiMode)` from topology setup code before any transactions are performed. `open` returns `false` on failure. The mode defaults to `SPI_MODE_CPOL_LOW_CPHA_LOW` (mode 0).
3. **Perform transactions** by invoking the `SpiWriteRead` port with equal-sized write and read buffers.

```cpp
// Topology configuration example
spiDriver.open(0, 0, Drv::SPI_FREQUENCY_1MHZ, Drv::SPI_MODE_CPOL_LOW_CPHA_LOW);
```

Note that the caller owns the buffers passed to each port; the driver does not retain or deallocate them.

## 5. Change Log

| Date | Description |
|---|---|
| 2026-08-10 | Initial SDD |
