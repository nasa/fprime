# Develop a Device Driver

This document describes the steps to create a new device driver in F Prime. The guide walks through the development of a device driver for an IMU sensor (MPU6050) connected over I2C, however the methodology generalizes to other types of devices and buses.

---

## Prerequisites

Before starting, you should have:

* Completed the [LedBlinker Tutorial](https://fprime.jpl.nasa.gov/latest/tutorials-led-blinker/docs/led-blinker/).
* A general understanding of [FPP component modeling](https://nasa.github.io/fpp/fpp-users-guide.html).
* Experience creating commands, events, and telemetry in FPP.
* A working build of F Prime on your system (`fprime-util` runs successfully).

---

## Application-Manager-Driver Pattern

A "device driver" traditionally refers to the entire stack of software that manages a hardware device. In F´, the driver-manager pattern splits this in two components: the device manager component and the bus driver component. The bus driver handles the platform-specific implementation of communications on a specific bus (e.g., LinuxI2cDriver, LinuxUartDriver). The device manager handles the operations and logic for a specific device. This enhances modularity and reusability: for example the same device manager can be ported to different platforms by switching the bus driver component.

Please refer to the [Application Manager Driver pattern documentation](../user-manual/design-patterns/app-man-drv.md) for more details on the design pattern used in F Prime for device drivers.

### Example and reference

Consider an [MPU6050 IMU sensor](https://cdn-learn.adafruit.com/downloads/pdf/mpu6050-6-axis-accelerometer-and-gyro.pdf) connected via I2C. An example instantiation of the Application-Manager-Driver pattern, defined in the fprime-sensors repository (see [MpuImu component](https://github.com/fprime-community/fprime-sensors/tree/devel/fprime-sensors/MpuImu)), would look like this:

- The bus driver component (LinuxI2cDriver on Linux) handles I2C read and write operations at arbitrary addresses.
- The device manager component (ImuManager) uses the bus driver layer to implement the specific data read/writes sequences that produce relevant data for the MPU6050 sensor, as per its datasheet.
- The application layer uses the device manager component to obtain sensor data when needed.

```mermaid
graph LR
    subgraph SW["F´ Software"]
        A[Application Layer] -->|Read sensor data| B[ImuManager<br/>Device Manager]
        B -->|I2C read/write commands| C[LinuxI2cDriver<br/>Bus Driver]
    end
    subgraph HW["Hardware"]
        D[MPU6050 IMU Sensor<br/>I2C Device]
    end
    C -->|Hardware I/O| D
    style A fill:#e1f5ff
    style B fill:#fff4e1
    style C fill:#f0e1ff
    style D fill:#f5f5f5
```
**Figure**: Application-Manager-Driver pattern example with MPU6050 IMU sensor over I2C.

> [!NOTE]
> The reference MpuImu component linked above is implemented using a state machine to manage the device's initialization and operational modes. This is a design choice for this specific component and **not** a requirement for all device managers. Simpler devices may not need a state machine. Other device manager examples are available in [https://github.com/fprime-community/fprime-sensors/tree/devel/fprime-sensors](https://github.com/fprime-community/fprime-sensors/tree/devel/fprime-sensors).

---

## How-To Develop a Device Manager

This section focuses on the device manager component. The bus driver component is assumed to already exist, and its implementation is covered in a [separate section](#how-to-develop-a-bus-driver) of this guide. Linux implementations are available in core F´ with for example `Drv.LinuxUartDriver`, `Drv.LinuxI2cDriver` and `LinuxSpiDriver`.

### Step 1 - Understand the Hardware

Before starting development, obtain the datasheet and any relevant documentation for the hardware device. Understand its communication protocol, data formats, and anything relevant to your needs when interfacing with it.

### Step 2 - Define the Device Manager Component

Use `fprime-util new --component` to create a new component for your device manager. It is often useful (although not required) to use Events and Telemetry. It is often sufficient to start with a `passive` component, and upgrade to `active` or `queued` later if needed.

This component will translate device-specific operations into bus transactions. Identify the bus type (I2C, SPI, UART, etc.) and the operations needed (read, write, configure, etc.). These should be reflected in the component's ports by mirroring the bus driver's interface.

For our `ImuManager` component, we are using an I2C bus, therefore we need to define ports that mirror the `Drv.I2c` interface (see [Drv/Interfaces/I2c.fpp](../../Drv/Interfaces/I2c.fpp)).  
For example, a `Drv.I2c` component provides an ***input*** port of type `Drv.I2cWriteRead`, so we need to define an ***output*** port of that type in our component in order to connect to a bus driver component. We mirror each Bus Driver port that we need to use in our Device Manager.

```python
# In: ImuManager.fpp
@ Component emitting telemetry read from an MpuImu
passive component ImuManager {
    @ Output port allowing to connect to an I2c bus driver for writeRead operations
    output port busWriteRead: Drv.I2cWriteRead

    @ Output port allowing to connect to an I2c bus driver for write operations
    output port busWrite: Drv.I2c

    # We could also mirror the Drv.I2c 'read' port if needed
    # but we do not need them for this example
}
```

### Step 3 - Implement Device-Specific Behavior

It is good practice to create helper functions for device operations, based on your datasheet. These helpers will then be called from your component's port handlers to respond to requests, or for example update telemetry on a schedule.

```cpp
// In: ImuManager.hpp
class ImuManager final : public ImuManagerComponentBase {
  public:
    // Register addresses (from datasheet)
    static constexpr U8 RESET_REG = 0x00;
    static constexpr U8 CONFIG_REG = 0x01;
    static constexpr U8 DATA_REG = 0x10;

    // Register values
    static constexpr U8 RESET_VAL = 0x80;
    static constexpr U8 DEFAULT_ADDR = 0x48;
    static constexpr U8 DATA_SIZE = 6;

    // [... other component code ...]
};
```

```cpp
// In: ImuManager.cpp

// Reset device
Drv::I2cStatus ImuManager::reset() {
    U8 cmd[] = {RESET_REG, RESET_VAL};  // From your datasheet
    Fw::Buffer writeBuffer(cmd, sizeof(cmd));
    // Port call to bus driver to write the buffer
    return this->busWrite_out(0, m_address, writeBuffer);
}

// Read sensor data
Drv::I2cStatus ImuManager::read(ImuData& output_data) {
    U8 regAddr = DATA_REG;
    U8 rawData[DATA_SIZE];
    Fw::Buffer writeBuffer(&regAddr, 1);
    Fw::Buffer readBuffer(rawData, DATA_SIZE);
    // Port call to bus driver to write register address and read data
    Drv::I2cStatus status = this->busWriteRead_out(0, m_address, writeBuffer, readBuffer);
    if (status == Drv::I2cStatus::I2C_OK) {
        // Convert to engineering units - implement as per your datasheet
        output_data = convertRawData(rawData);
    }
    return status;
}
```

> [!TIP]
> The above code snippets are simplified for clarity. In a concrete implementation, these methods and constants would be private members of the component class. Helpers can be broken out in a different file if desired. This is all up to the implementer.

### Step 4 - Expose Behavior to Application layer

Once the device-specific helper functions are implemented, integrate them into your component's behavior. For example, we can configure our ImuManager to:
- a) Emit telemetry on a schedule by connecting to a RateGroup
- b) Expose data to the application layer through additional ports

>[!NOTE]
> Functionalities (a) and (b) are shown for illustrative purposes. You may not need to implement both telemetry and data ports depending on your requirements and use case.

First, let's represent our ImuData in FPP so we can use it in telemetry and ports:

```python
# In: MyProject/Types/ImuTypes.fpp
@ Struct representing X, Y, Z data
struct GeometricVector3 {
    x: F32
    y: F32
    z: F32
}
@ Struct representing IMU data
struct ImuData {
    acceleration: GeometricVector3
    rotation: GeometricVector3
    temperature: F32
}
```

**a) Emit telemetry on a schedule**

Add a run port to connect to a RateGroup, and implement the run handler to read data and emit telemetry on a regular cadence:
```python
# In: Components/ImuManager/ImuManager.fpp
passive component ImuManager {
    [... other code ...]

    @ Telemetry channel for IMU data (struct of acceleration, rotation, temperature)
    telemetry ImuData: ImuData
    
    @ Scheduling port for reading from IMU and writing to telemetry
    sync input port run: Svc.Sched

    @ Event for logging I2C read errors
    event ImuReadError(status: Drv.I2cStatus) severity warning high format "I2C read error with status {}"
}
```

```cpp
// In: Components/ImuManager/ImuManager.cpp
void ImuManager::run_handler(FwIndexType portNum, U32 context) {
    ImuData data;
    Drv::I2cStatus status = this->read(data);
    // Check status and emit telemetry or log error
    if (status == Drv::I2cStatus::I2C_OK) {
        this->tlmWrite_ImuData(data);
    } else {
        this->log_WARNING_HI_ImuReadError(status);
    }
}   
```

**b) Expose data to application layer**

Add a port that returns data on request:

```python
# In: Components/ImuManager/ImuManager.fpp
@ Port to read IMU data on request. Update data reference and return status
port ImuDataRead(ref data: ImuData) -> Fw.Success

passive component ImuManager {
    [... other code ...]

    sync input port getData: ImuDataRead
}
```

```cpp
// In: Components/ImuManager/ImuManager.cpp
Fw::Success ImuManager::getData_handler(FwIndexType portNum, ImuData& data) {
    Drv::I2cStatus status = this->read(data);
    return (status == Drv::I2cStatus::I2C_OK) ? Fw::SUCCESS : Fw::FAILURE;
}   
```

>[!TIP]
> For more complex use cases, it is recommended not to use Fw.Success but rather define your own status enum to represent different error conditions. Examples are available in the `Drv/` directory in F Prime: [Drv.I2cStatus](https://github.com/nasa/fprime/blob/3f25d8b5358c6a734029e7eeb9dd8621e8895c90/Drv/Ports/I2cDriverPorts.fpp#L14-L21) and [Drv.GpioStatus](https://github.com/nasa/fprime/blob/3f25d8b5358c6a734029e7eeb9dd8621e8895c90/Drv/Ports/GpioDriverPorts.fpp#L2-L7).

### Step 5 - Integrate into Deployment

Wire your device manager to the bus driver in a topology:

```python
# In: topology.fpp
instance imuManager: MyProject.ImuManager base id 0x1000
instance busDriver: Drv.LinuxI2cDriver base id 0x2000

topology MyTopology {
    connections {
        imuManager.busWriteRead -> busDriver.writeRead
    }
}
```

Then configure the bus driver to open the correct device. This is platform specific. On Linux, this may look like the following:

```cpp
// In: Topology.cpp
void configureTopology() {
    ...

    Drv::I2cStatus status = busDriver.open("/dev/i2c-1"); // Or use CLI args
    // TODO: handle status, log if error

    // Optionally, if needed, this is where you would configure the device address
    // This method would need to be implemented in your device manager
    Drv::I2cStatus status = imuManager.configure(0x68); // Device I2C address from datasheet
}
```

> [!TIP]
> A reference MpuImuManager component implementation is available in the fprime-sensors repository: [MpuImu component reference](https://github.com/fprime-community/fprime-sensors/tree/devel/fprime-sensors/MpuImu/Components/ImuManager)

---

## How-To Develop a Bus Driver

This section focuses on the bus driver component. The bus driver handles communication over a specific bus (I2C, SPI, UART, etc.). If a suitable bus driver already exists in F Prime (this is the case for most common buses on Linux, see inside the `fprime/Drv/` package), you can skip this section. As mentioned earlier, the bus driver's role is to provide a generic interface for read/write operations over a specific bus that a device manager can use. By splitting the bus driver into its own component, we can (a) re-use the same bus driver implementation for multiple device managers, and (b) swap out bus drivers when porting to different platforms, but re-using the same device manager logic.

In this section, we will keep working with our example MPU6050 IMU sensor connected over I2C. Our goal will be to implement a bus driver for I2C communication on [Zephyr RTOS](https://zephyrproject.org/) instead of Linux. The methodology generalizes to other buses and platforms.

### Step 1 - Understand the bus protocol and platform APIs

Before starting development, understand the bus communication protocol you are targeting (I2C, SPI, UART, etc.) and obtain documentation for the platform-specific APIs for that protocol. Understand how to perform read and write operations on the bus using the platform's SDK or libraries. 

In our case, we need to understand how to perform I2C read and write operations using Zephyr's I2C API. We look to the [Zephyr I2C documentation](https://docs.zephyrproject.org/latest/reference/peripherals/i2c.html) and [Zephyr I2C API](https://docs.zephyrproject.org/latest/doxygen/html/group__i2c__interface.html). It can also be useful to look at existing Zephyr code samples that use I2C.

We learn the following:
* Zephyr uses the [`device`](https://docs.zephyrproject.org/latest/doxygen/html/structdevice.html) struct to identify an I2C device. These can be retrieved from the Device Tree through macros (see [Zephyr Device Tree How-To](https://docs.zephyrproject.org/latest/build/dts/howtos.html)).
* With a `device` instance, we can use the `i2c_write`, `i2c_read` and `i2c_write_read` functions to perform I2C write and read operations (see [API](https://docs.zephyrproject.org/latest/doxygen/html/group__i2c__interface.html#ga2cc5f49493dce89e128ccbfa9d6149a0)).


### Step 2 - Define the Bus Driver Component

Use `fprime-util new --component` to create a new component for your bus driver. The set of ports that a bus driver needs to expose depends on the bus communication protocol (I2C, SPI, UART, etc.). F Prime provides standard interfaces for common bus types in the `Drv/Interfaces/` directory. For I2C, we can use the existing `Drv.I2c` interface (see [Drv/Interfaces/I2c.fpp](../../Drv/Interfaces/I2c.fpp)).

```python
# In: ZephyrI2cDriver.fpp
@ I2C bus driver interface
passive component ZephyrI2cDriver {
    # This imports the Drv.I2c interface, adding the required ports to this component
    import Drv.I2c
}
```

> [!TIP]
> Our I2C bus driver will only be responding to read/write requests from a device manager, therefore we define it as a `passive component` and the `Drv.I2c` ports are sufficient. If your bus driver needs to perform scheduled tasks (e.g., polling, timeouts, etc.), you may consider adding a scheduling port (`Svc.Sched`) to hook to a [Svc.RateGroup](../../Svc/ActiveRateGroup/docs/sdd.md), and potentially switching to an `active` component. `queued` components can also be used but need careful design to ensure messages are dispatched.

Run `fprime-util impl` to generate the component C++, including the port handler to fill out. In our case, we will need to implement the `write`, `read`, and `writeRead` port handlers.

### Step 3 - Allow for bus configuration on startup

Bus drivers will most likely require configuration on startup, usually done by the project inside `configureTopology()`. This can include opening the bus device, selecting pin numbers, setting baud rates, or other parameters. For example, during the LedBlinker tutorial, we had to configure the GPIO driver with the correct pin number and other parameters (see [LedBlinkerTopology.cpp](https://github.com/fprime-community/fprime-workshop-led-blinker/blob/9147623edd1cb7df0786a60b549a12599f6f59eb/LedBlinker/LedBlinkerDeployment/Top/LedBlinkerDeploymentTopology.cpp#L57)). This allows the same component implementation to be reused for multiple devices: you don't want to hardcode device paths or pin numbers in the bus driver itself. Instead, each instance of the component is configured at runtime to open the user-specified device.

For our ZephyrI2cDriver, we will implement a public `open()` method that takes an `device` structure to identify the I2C device. This method will store the `device` as a member variable for later use in read/write operations.

```cpp
// In: ZephyrI2cDriver.cpp
Drv::I2cStatus ZephyrI2cDriver::open(const struct device* i2c_device) {
    this->m_device = i2c_device;
    if (!device_is_ready(this->m_device)) {
        return Drv::I2cStatus::I2C_OPEN_ERR;
    }
    return Drv::I2cStatus::I2C_OK;
}
```

With this method, projects can now configure the bus driver in `configureTopology()`:

```c++
// In: Topology.cpp
#include <zephyr/device.h>
static const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));

void configureTopology() {
    Drv::I2cStatus status = i2cDriver.open(i2c_dev);
    if (status != Drv::I2cStatus::I2C_OK) {
        Fw::Logger::log("[I2C] Failed to open I2C device\n");
    } else {
        Fw::Logger::log("[I2C] I2C device opened successfully\n");
    }
    ...
}
```

### Step 4 - Implement Bus Operations

Implement the port calls that are part of the bus driver interface. In our case, `Drv.I2c` contains `write`, `read`, and `writeRead` port handlers, for which the function signatures are autocoded by `fprime-util impl`. With the Zephyr I2C API, this may look like the following:

```cpp
// In: ZephyrI2cDriver.cpp
Drv::I2cStatus ZephyrI2cDriver ::read_handler(FwIndexType portNum, U32 addr, Fw::Buffer& buffer) {
    int status = i2c_read(this->m_device, buffer.getData(), buffer.getSize(), addr);
    if (status != 0) {
        return Drv::I2cStatus::I2C_READ_ERR;
    }
    return Drv::I2cStatus::I2C_OK;
}

Drv::I2cStatus ZephyrI2cDriver ::write_handler(FwIndexType portNum, U32 addr, Fw::Buffer& buffer) {
    int status = i2c_write(this->m_device, buffer.getData(), buffer.getSize(), addr);
    if (status != 0) {
        return Drv::I2cStatus::I2C_WRITE_ERR;
    }
    return Drv::I2cStatus::I2C_OK;
}

Drv::I2cStatus ZephyrI2cDriver ::writeRead_handler(FwIndexType portNum, U32 addr, Fw::Buffer& writeBuffer, Fw::Buffer& readBuffer) {
    int status = i2c_write_read(this->m_device, addr, writeBuffer.getData(), writeBuffer.getSize(),
                                readBuffer.getData(), readBuffer.getSize());
    if (status != 0) {
        return Drv::I2cStatus::I2C_WRITE_ERR;
    }
    return Drv::I2cStatus::I2C_OK;
}
```

### Step 5 - Swap Bus Driver in Deployment

Once a different bus driver is implemented, you can use it in your deployment topology. If you were testing your deployment in Linux, you can simply replace the LinuxI2cDriver with our ZephyrI2cDriver:

```diff
# In: topology.fpp
-  instance i2cDriver: LinuxI2cDriver base id 0x10015000
+  instance i2cDriver: Zephyr.ZephyrI2cDriver base id 0x10015000
```

And update the configuration code in `configureTopology()` to use the Zephyr-specific device opening method shown in Step 3.

```diff
// In: Topology.cpp
+ #include <zephyr/device.h>
+ static const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));

void configureTopology() {
-    bool status = i2cDriver.open("/dev/i2c-1"); // Linux open() call
+    Drv::I2cStatus status = i2cDriver.open(i2c_dev); // Zephyr open() call
    if (status != Drv::I2cStatus::I2C_OK) {
        Fw::Logger::log("[I2C] Failed to open I2C device\n");
    } else {
        Fw::Logger::log("[I2C] I2C device opened successfully\n");
    }
    ...
}
```

---

## Best Practices

- Use parameters for configurable device settings (ranges, modes, etc.)
- Always check bus operation status and emit events on errors
- Define all register addresses/values as named constants from the datasheet, don't use "magic" numbers
- Keep device-specific logic in helper functions separate from component infrastructure

---

## Additional Resources

- [Application Manager Driver Pattern](../user-manual/design-patterns/app-man-drv.md)
- [fprime-sensors Repository](https://github.com/fprime-community/fprime-sensors) - A collection of ready-to-use device managers for specific devices
- [fprime-sensors-reference Repository](https://github.com/fprime-community/fprime-sensors-reference) - Reference project that uses sensors defined in fprime-sensors
- [F´ core Linux Bus Drivers](../../Drv/)
- [fprime-zephyr package](https://github.com/fprime-community/fprime-zephyr) - F Prime support for Zephyr RTOS, including common bus drivers for Zephyr

