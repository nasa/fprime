# Drv::LinuxUartDriver

## 1. Introduction

The LinuxUartDriver component provides a Linux-specific implementation of a UART (Universal Asynchronous Receiver-Transmitter) serial communication driver. It implements the byte stream driver model interface (see [`Drv.ByteStreamDriver`](../../Interfaces/ByteStreamDriver.fpp)) to enable serial communication with external devices through UART ports on Linux systems.

The component wraps Linux termios API functionality to provide configurable serial communication with support for various baud rates, flow control options, and parity settings. It implements bidirectional communication using a dedicated receive thread and synchronous send operations.

For more information on the ByteStreamDriverModel see: [`Drv::ByteStreamDriverModel`](../../ByteStreamDriverModel/docs/sdd.md).

## 2. Requirements

| Name | Description | Validation |
|---|---|---|
| LINUX-UART-COMP-001 | The LinuxUartDriver component shall implement the Drv.ByteStreamDriver interface | inspection |
| LINUX-UART-COMP-002 | The LinuxUartDriver component shall provide configurable baud rates from 9600 to 4MHz | inspection |
| LINUX-UART-COMP-003 | The LinuxUartDriver component shall provide configurable flow control (none/hardware) | inspection |
| LINUX-UART-COMP-004 | The LinuxUartDriver component shall provide configurable parity (none/odd/even) | inspection |
| LINUX-UART-COMP-005 | The LinuxUartDriver component shall provide a dedicated read thread for receiving data | inspection |
| LINUX-UART-COMP-006 | The LinuxUartDriver component shall report telemetry for bytes sent and received | inspection |
| LINUX-UART-COMP-007 | The LinuxUartDriver component shall handle UART errors and report them via events | inspection |
| LINUX-UART-COMP-008 | The LinuxUartDriver component shall support buffer allocation for receive operations | inspection |

## 3. Design

The LinuxUartDriver component implements the design specified by the [`Drv.ByteStreamDriver`](../../Interfaces/ByteStreamDriver.fpp) interface.

### 3.1 Architecture

The component consists of the following key elements:

- **UART Configuration**: Handles device opening, baud rate, flow control, and parity settings using Linux termios API
- **Send Handler**: Synchronous transmission of data via the `send` port (guarded input port)
- **Receive Thread**: Asynchronous reception of data via a dedicated thread that calls the `recv` output port
- **Buffer Management**: Integration with F´ buffer allocation system for memory management
- **Telemetry Reporting**: Tracks and reports bytes sent and received statistics
- **Error Handling**: Comprehensive error detection and event reporting

### 3.2 Send Operation

When data is sent via the `send` input port:
1. The component validates the file descriptor and buffer
2. Data is written to the UART device using the Linux `write()` system call
3. Bytes sent counter is updated for telemetry
4. Status is returned indicating success or failure

### 3.3 Receive Operation

The receive operation runs in a separate thread:
1. A buffer is allocated from the buffer manager
2. The thread blocks on `read()` waiting for incoming data
3. Received data is packaged in the buffer and sent via `recv` output port
4. Bytes received counter is updated for telemetry
5. Errors are logged and reported via events

### 3.4 Threading Model

The component uses a single dedicated thread for receive operations (`serialReadTaskEntry`). This thread:
- Runs continuously until `quitReadThread()` is called
- Allocates buffers for each receive operation
- Handles timeouts and errors gracefully
- Can be started with configurable priority and stack size

## 4. Usage

The LinuxUartDriver must be configured with device parameters before use. The typical usage pattern is:

1. **Open Device**: Configure the UART device with desired parameters
2. **Start Receive Thread**: Begin the receive thread for incoming data
3. **Send/Receive Data**: Use the ByteStreamDriverModel ports for communication
4. **Shutdown**: Stop the receive thread and close the device

### 4.1 Configuration Example

The LinuxUartDriver should be instantiated in the FPP topology and configured using separate functions following F´ patterns:

```cpp
// Configuration function - called during topology setup
void configureTopology() {
    // Open UART device with configuration
    bool success = uart.open("/dev/ttyUSB0",                    // Device path
                             Drv::LinuxUartDriver::BAUD_115K,   // 115200 baud rate
                             Drv::LinuxUartDriver::NO_FLOW,     // No flow control
                             Drv::LinuxUartDriver::PARITY_NONE, // No parity
                             1024);                             // Buffer size
    if (!success) {
        // Handle configuration error
    }
    ...
}

// Startup function - called when starting tasks
void setupTopology() {
    // Start receive thread
    uart.start(UART_PRIORITY,           // Thread priority
               32 * 1024,               // Thread stack size
               Os::Task::TASK_DEFAULT); // Thread CPU affinity mask
}

// Shutdown function - called during teardown
void teardownTopology() {
    uart.quitReadThread();
    uart.join();
}
```

### 4.2 Integration with Rate Groups

The component includes a `run` input port for telemetry reporting that should be connected to a rate group in the FPP topology:

```fpp
# In topology.fpp connections section
connections RateGroups {
  # Connect UART driver to rate group for telemetry
  rateGroup1Comp.RateGroupMemberOut[N] -> uart.run
}
```

## 5. Configuration

### 5.1 Device Parameters

| Parameter | Type | Description | Valid Values |
|-----------|------|-------------|--------------|
| device | const char* | Path to UART device | Linux device path (e.g., "/dev/ttyUSB0") |
| baud | Drv::LinuxUartDriver::UartBaudRate | Communication baud rate | See baud rate enumeration |
| fc | Drv::LinuxUartDriver::UartFlowControl | Flow control setting | NO_FLOW, HW_FLOW |
| parity | Drv::LinuxUartDriver::UartParity | Parity setting | PARITY_NONE, PARITY_ODD, PARITY_EVEN |
| allocationSize | FwSizeType | Receive buffer size | Positive integer (bytes) |

### 5.2 Baud Rate Options

The component supports the following baud rates:

| Enumeration | Numeric Value | Availability |
|-------------|---------------|--------------|
| BAUD_9600 | 9600 | All platforms |
| BAUD_19200 | 19200 | All platforms |
| BAUD_38400 | 38400 | All platforms |
| BAUD_57600 | 57600 | All platforms |
| BAUD_115K | 115200 | All platforms |
| BAUD_230K | 230400 | All platforms |
| BAUD_460K | 460800 | Linux only |
| BAUD_921K | 921600 | Linux only |
| BAUD_1000K | 1000000 | Linux only |
| BAUD_1152K | 1152000 | Linux only |
| BAUD_1500K | 1500000 | Linux only |
| BAUD_2000K | 2000000 | Linux only |
| BAUD_2500K | 2500000 | Linux only (if supported) |
| BAUD_3000K | 3000000 | Linux only (if supported) |
| BAUD_3500K | 3500000 | Linux only (if supported) |
| BAUD_4000K | 4000000 | Linux only (if supported) |

### 5.3 Thread Configuration

The receive thread can be configured with:

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| priority | FwTaskPriorityType | TASK_PRIORITY_DEFAULT | Thread priority |
| stackSize | Os::Task::ParamType | TASK_DEFAULT | Thread stack size |
| cpuAffinity | Os::Task::ParamType | TASK_DEFAULT | CPU affinity mask |

### 5.4 Events and Telemetry

The component generates the following events:
- **OpenError**: UART device open failures
- **ConfigError**: UART configuration failures
- **WriteError**: Data transmission errors
- **ReadError**: Data reception errors
- **PortOpened**: Successful device configuration
- **NoBuffers**: Buffer allocation failures
- **BufferTooSmall**: Insufficient buffer size

The component reports the following telemetry:
- **BytesSent**: Total bytes transmitted
- **BytesRecv**: Total bytes received