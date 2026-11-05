# Drv::LinuxI2cDriver

## 1. Introduction

The LinuxI2cDriver component provides a Linux-specific implementation of an I2C (Inter-Integrated Circuit) bus master driver. It implements the [`Drv.I2c`](../../Interfaces/I2c.fpp) interface, exposing guarded synchronous ports for writing to, reading from, and performing combined write/read transactions with I2C slave devices.

The component wraps the Linux userspace I2C device interface (`/dev/i2c-*`, via `ioctl` with `I2C_RDWR` and `read`/`write` system calls) to perform bus transactions on behalf of client components.

## 2. Requirements

| Name | Description | Validation |
|---|---|---|
| LINUX-I2C-COMP-001 | The LinuxI2cDriver component shall implement the Drv.I2c interface | inspection |
| LINUX-I2C-COMP-002 | The LinuxI2cDriver component shall support opening a Linux I2C device by device path | inspection |
| LINUX-I2C-COMP-003 | The LinuxI2cDriver component shall perform synchronous write transactions to a specified 7-bit slave address | inspection |
| LINUX-I2C-COMP-004 | The LinuxI2cDriver component shall perform synchronous read transactions from a specified 7-bit slave address | inspection |
| LINUX-I2C-COMP-005 | The LinuxI2cDriver component shall perform combined write/read transactions without releasing the bus between the write and the read | inspection |
| LINUX-I2C-COMP-006 | The LinuxI2cDriver component shall return an `I2cStatus` value indicating the result of each transaction | inspection |

## 3. Design

The LinuxI2cDriver is a passive component. All operations execute synchronously on the caller's thread; the `Drv.I2c` interface ports are guarded, so concurrent transactions from multiple callers are serialized by the component's mutex.

### 3.1 Port Description

| Port | Kind | Type | Description |
|---|---|---|---|
| `write` | guarded input | `Drv.I2c` | Write the contents of a buffer to a slave device |
| `read` | guarded input | `Drv.I2c` | Read from a slave device into a buffer |
| `writeRead` | guarded input | `Drv.I2cWriteRead` | Write then read in a single bus transaction |

### 3.2 Transaction Handling

1. Each handler validates that the device file descriptor is open, returning `I2cStatus::I2C_OPEN_ERR` otherwise.
2. `write` and `read` handlers select the slave address with `ioctl(I2C_SLAVE)` (failure yields `I2C_ADDRESS_ERR`) and issue `write()`/`read()` system calls sized by the provided `Fw::Buffer` (failures yield `I2C_WRITE_ERR`/`I2C_READ_ERR`).
3. The `writeRead` handler issues a combined transaction via `ioctl(I2C_RDWR)`, so the write and read occur back-to-back with a repeated start condition. This is required by devices that lose register-pointer state if the bus is released between the address write and the data read. Because the combined transaction reports a single result, failures are reported as `I2C_OTHER_ERR`.

### 3.3 Stub Implementation

A stub implementation (`LinuxI2cDriverStub.cpp`) is provided for platforms without Linux I2C support; it reports open failures for all transactions. The build selects the real or stub implementation based on the target platform.

## 4. Usage

The typical usage pattern is:

1. **Instantiate** the component in the topology.
2. **Open the device** by calling `open("/dev/i2c-<N>")` from topology setup code before any transactions are performed. `open` returns `false` on failure.
3. **Perform transactions** by invoking the `write`, `read`, or `writeRead` ports with the slave address and data buffer(s).

```cpp
// Topology configuration example
i2cDriver.open("/dev/i2c-1");
```

Note that the caller owns the buffers passed to each port; the driver does not retain or deallocate them.

## 5. Change Log

| Date | Description |
|---|---|
| 2026-08-10 | Initial SDD |
