# Drv::LinuxGpioDriver

## 1. Introduction

The LinuxGpioDriver component provides a Linux-specific implementation of a General Purpose Input/Output (GPIO) pin driver. It implements the [`Drv.Gpio`](../../Interfaces/Gpio.fpp) interface to enable reading, writing, and interrupt detection on a single GPIO line.

The component wraps the Linux GPIO character device ABI (`/dev/gpiochip*`), configuring and accessing a line through `ioctl` requests rather than the deprecated sysfs interface. Each component instance manages exactly one GPIO line, and that line is fixed to a single mode chosen at configuration time.

For more information on the driver interfaces see: [`Drv FPP Interfaces`](../../Interfaces/docs/sdd.md).

## 2. Requirements

| Name | Description | Validation |
|---|---|---|
| LINUX-GPIO-COMP-001 | The LinuxGpioDriver component shall implement the Drv.Gpio interface | inspection |
| LINUX-GPIO-COMP-002 | The LinuxGpioDriver component shall configure a GPIO line as input, output, or interrupt using the Linux GPIO character device | inspection |
| LINUX-GPIO-COMP-003 | The LinuxGpioDriver component shall set a caller-supplied default state when a line is configured as output | inspection |
| LINUX-GPIO-COMP-004 | The LinuxGpioDriver component shall reject read and write requests that do not match the configured mode | inspection |
| LINUX-GPIO-COMP-005 | The LinuxGpioDriver component shall detect line transitions on a rising edge, a falling edge, or both edges as configured | inspection |
| LINUX-GPIO-COMP-006 | The LinuxGpioDriver component shall provide a dedicated thread for interrupt detection | inspection |
| LINUX-GPIO-COMP-007 | The LinuxGpioDriver component shall emit a timestamped interrupt on the gpioInterrupt output port when a configured transition occurs | inspection |
| LINUX-GPIO-COMP-008 | The LinuxGpioDriver component shall report configuration and runtime errors via events | inspection |

## 3. Design

LinuxGpioDriver is a passive component implementing the design specified by the [`Drv.Gpio`](../../Interfaces/Gpio.fpp) interface.

### 3.1 Port Descriptions

| Name | Type | Kind | Description |
|---|---|---|---|
| gpioRead | [`Drv.GpioRead`](../../Ports/GpioDriverPorts.fpp) | sync input | Reads the current logic state of the line |
| gpioWrite | [`Drv.GpioWrite`](../../Ports/GpioDriverPorts.fpp) | sync input | Sets the logic state of the line |
| gpioInterrupt | `Svc.Cycle` | output | Emits a timestamp when a configured line transition is detected |

The component declares no commands, telemetry channels, or parameters. Since the component is passive, `gpioRead` and `gpioWrite` execute on the caller's thread.

### 3.2 Architecture

The component consists of the following key elements:

- **Chip and Line Configuration**: Opens the `/dev/gpiochip*` device, validates the requested line, and requests either a line handle or a line event from the kernel
- **Read/Write Handlers**: Synchronous access to the line value via the `gpioRead` and `gpioWrite` ports
- **Interrupt Thread**: An optional thread that polls the line event descriptor and drives the `gpioInterrupt` output port
- **Status Translation**: Maps `errno` values from the underlying system calls onto `Os::File::Status` and `Drv::GpioStatus`
- **Error Reporting**: Reports open, polling, and read failures via events

### 3.3 Pin Configuration

A line is configured once through the `open()` method, which selects one of five modes:

| Configuration | Direction | Supported Operations |
|---|---|---|
| GPIO_OUTPUT | Output | `gpioWrite` |
| GPIO_INPUT | Input | `gpioRead` |
| GPIO_INTERRUPT_RISING_EDGE | Input | `gpioInterrupt` on low-to-high transition |
| GPIO_INTERRUPT_FALLING_EDGE | Input | `gpioInterrupt` on high-to-low transition |
| GPIO_INTERRUPT_BOTH_RISING_AND_FALLING_EDGES | Input | `gpioInterrupt` on either transition |

`open()` performs the following steps:

1. Opens the GPIO chip device and retrieves chip information
2. Verifies that the requested line number exists on that chip
3. Retrieves line information, including the name and any current consumer
4. Requests a line handle (`GPIO_GET_LINEHANDLE_IOCTL`) for the input and output modes, or a line event (`GPIO_GET_LINEEVENT_IOCTL`) for the interrupt modes
5. Retains the resulting file descriptor and configuration on success

The component name is passed to the kernel as the consumer label for the line (when object names are enabled via `FW_OBJECT_NAMES`), so the owning component is identifiable in tools such as `gpioinfo`.

### 3.4 Read and Write Operations

The read and write handlers are strictly gated on the configured mode:

- `gpioRead` returns the line state only when the line is configured as `GPIO_INPUT`
- `gpioWrite` sets the line state only when the line is configured as `GPIO_OUTPUT`

A request that does not match the configured mode returns `Drv::GpioStatus::INVALID_MODE` and performs no hardware access. Notably, the interrupt configurations do not support `gpioRead`; a line that must be both polled and interrupt-driven requires a separate component instance.

### 3.5 Interrupt Detection

Interrupt detection runs on a dedicated thread started by `start()`. The thread is only available in the three interrupt configurations; calling `start()` in any other mode returns `Drv::GpioStatus::INVALID_MODE` and starts no thread.

The polling loop performs the following:

1. Polls the line event descriptor for readable data, using a fixed `GPIO_POLL_TIMEOUT` of 500 milliseconds
2. Reads the kernel event record when the descriptor becomes ready
3. Captures a timestamp with `Os::RawTime`
4. Invokes the `gpioInterrupt` output port with that timestamp
5. Reports short reads, polling failures, and timestamp failures via events; on a timestamp failure the interrupt is still emitted with the (possibly invalid) timestamp

Because `gpioInterrupt` is invoked directly from the polling thread, the receiving component is responsible for any thread-safety and execution-time constraints on that path. Connecting the port to an `async` input port, such as `CycleIn` on `Svc::ActiveRateGroup`, keeps work off the polling thread.

### 3.6 Threading Model

The interrupt thread is controlled by three methods:

- `start()`: sets the running flag and starts the polling task; returns `Drv::GpioStatus::UNKNOWN_ERROR` if the task fails to start
- `stop()`: clears the running flag, requesting shutdown
- `join()`: blocks until the polling task exits

The running flag is guarded by a mutex. The poll timeout bounds how long the loop can block, so the thread observes a `stop()` request and exits within one timeout period. `stop()` must be called before `join()`, otherwise `join()` will not return.

### 3.7 Platform Support

Outside of the stubbed build, this component is restricted to Linux targets. When the `FPRIME_USE_STUBBED_DRIVERS` CMake option is set, a stub implementation is built instead, allowing topologies that instantiate this component to build on platforms without GPIO character device support. The stub returns `Os::File::Status::NOT_SUPPORTED` from `open()` and `Drv::GpioStatus::UNKNOWN_ERROR` from the port handlers, and its polling loop delays instead of accessing hardware.

## 4. Usage

The LinuxGpioDriver must be configured before use. Each instance drives exactly one GPIO line, so a topology using several lines instantiates several components.

### 4.1 Configuration Example

The component should be instantiated in the FPP topology and configured using separate functions following F´ patterns:

```cpp
// Configuration function - called during topology setup
void configureTopology() {
    // Configure an output pin, driven low until written
    Os::File::Status status = gpioLed.open("/dev/gpiochip0",                     // GPIO chip device
                                           17,                                   // Line number on that chip
                                           Drv::LinuxGpioDriver::GPIO_OUTPUT,    // Pin configuration
                                           Fw::Logic::LOW);                      // Default output state
    if (status != Os::File::Status::OP_OK) {
        // Handle configuration error
    }

    // Configure an interrupt pin
    status = gpioButton.open("/dev/gpiochip0", 27, Drv::LinuxGpioDriver::GPIO_INTERRUPT_RISING_EDGE);
    if (status != Os::File::Status::OP_OK) {
        // Handle configuration error
    }
    ...
}

// Startup function - called when starting tasks
void setupTopology() {
    // Start the interrupt thread; only valid for interrupt configurations
    Drv::GpioStatus gpioStatus = gpioButton.start(GPIO_PRIORITY,           // Thread priority
                                                  Os::Task::TASK_DEFAULT,  // Thread stack size
                                                  Os::Task::TASK_DEFAULT); // Thread CPU affinity mask
    if (gpioStatus != Drv::GpioStatus::OP_OK) {
        // Handle startup error
    }
}

// Shutdown function - called during teardown
void teardownTopology() {
    gpioButton.stop();
    gpioButton.join();
}
```

### 4.2 Topology Connections

The read and write ports are connected to the component that uses the line, and an interrupt line is connected to a consumer of `Svc.Cycle`:

```fpp
# In topology.fpp connections section
connections Gpio {
  # A user component drives an output pin
  ledManager.gpioWrite -> gpioLed.gpioWrite

  # Interrupt pin drives a rate group
  gpioButton.gpioInterrupt -> buttonRateGroup.CycleIn
}
```

## 5. Configuration

### 5.1 Open Parameters

| Parameter | Type | Description | Valid Values |
|---|---|---|---|
| device | const char* | Path to the GPIO chip device | Linux device path (e.g. "/dev/gpiochip0") |
| gpio | U32 | Line number on the given chip | Less than the line count reported by the chip |
| configuration | Drv::LinuxGpioDriver::GpioConfiguration | Pin mode | See pin configuration table |
| default_state | Fw::Logic | Initial state for output pins | Fw::Logic::LOW (default), Fw::Logic::HIGH |

### 5.2 Thread Configuration

The interrupt thread can be configured with:

| Parameter | Type | Default | Description |
|---|---|---|---|
| priority | FwTaskPriorityType | TASK_PRIORITY_DEFAULT | Thread priority |
| stackSize | FwSizeType | TASK_DEFAULT | Thread stack size |
| cpuAffinity | FwSizeType | TASK_DEFAULT | CPU affinity mask |
| identifier | FwTaskIdType | TASK_DEFAULT | Task identifier |

### 5.3 Status Codes

Port handlers return `Drv::GpioStatus`:

| Status | Meaning |
|---|---|
| OP_OK | Operation succeeded |
| NOT_OPENED | Pin was never opened |
| INVALID_MODE | Operation not permitted with the current configuration |
| UNKNOWN_ERROR | An unknown error occurred |

`open()` returns `Os::File::Status`, translated from the `errno` reported by the underlying system call. Note: in the current implementation, a request for a line number beyond the chip's line count logs `OpenPinError` but returns the preceding status (`OP_OK`); callers should not rely on `open()` returning an error in that case.

### 5.4 Events

The component generates the following events:

| Event | Severity | Description |
|---|---|---|
| OpenChip | diagnostic | Chip and line configured successfully |
| OpenChipError | warning high | GPIO chip device could not be opened or queried |
| OpenPinError | warning high | GPIO line could not be configured |
| InterruptReadError | warning high | Interrupt event record read returned an unexpected size |
| PollingError | warning high | Interrupt polling returned an error |
| InterruptTimeError | warning high | Interrupt timestamp could not be read |
