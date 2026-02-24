# How-To: Implement a Radio Manager Component

This guide provides step-by-step instructions for implementing a radio manager component using the [Svc.Com Communications Adapter Interface](../reference/communication-adapter-interface.md). A radio manager handles communication with radio hardware, managing both outgoing transmissions (downlink) and incoming receptions (uplink).

---

## Prerequisites

Before starting, you should have:

- Completed the [LedBlinker Tutorial](https://fprime.jpl.nasa.gov/latest/tutorials-led-blinker/docs/led-blinker/).
- A general understanding of [FPP component modeling](https://nasa.github.io/fpp/fpp-users-guide.html).
- Experience creating commands, events, and telemetry in FPP.
- Read the [Communication Adapter Interface](../reference/communication-adapter-interface.md) reference documentation.

---

## Overview

A radio manager component bridges the F´ communication stack and radio hardware. To integrate with F´ standard uplink and downlink components, the radio manager must implement the [Communications Adapter Interface](../reference/communication-adapter-interface.md).

A common approach is to use an intermediate driver component (e.g., `Drv.ByteStreamDriver`) to communicate with radio hardware. This provides modularity and allows the same radio manager to work across different platforms by swapping the underlying driver.

> [!NOTE]
> For the curious reader: Some systems may not have a radio, for example if communications are done over a wired interface or local network (e.g. TCP/UDP). In this case, a component must still exist to implement the Communications Adapter Interface and act as a bridge to whichever hardware interface is used. An example of such a component is `Svc.ComStub` - it implements the Communications Adapter Interface and acts as a pass-through to a `Drv.ByteStreamDriver` for read/writes. This is the default configuration when creating new deployments! And because F´ ships with 3 common ByteStreamDriver implementations (`TcpClient`, `TcpServer` and `UART`), that is why you have three communication driver options to choose from when creating a new deployment.

## Step 0 - Read the Reference Documentation

Before starting, read the [Communication Adapter Interface](../reference/communication-adapter-interface.md) reference documentation. This How-To Guide is essentially walking through the implementation of the interface, and much of the information we are going to cover is explained at length in the reference documentation.

## Step 1 - Component Definition

Create a new component using `fprime-util new --component`. Define the component to import the `Svc.Com` interface. Next, identify the hardware communication protocol you will be using to interface with the radio hardware, and add the ports required to connect to a driver of said protocol. For our example, we will be using a UART radio, and therefore we are connecting to a UART driver (`ByteStreamDriver` interface).

```python
module MyProject {
    @ Radio manager implementing the Communications Adapter Interface
    passive component RadioManager {
        # Import the Communications Adapter Interface
        import Svc.Com

        # Import ports to connect to a UART driver (ByteStreamDriver)
        import Drv.ByteStreamDriverClient
    }
}
```

See [`Svc.ComStub`](../../Svc/ComStub/ComStub.fpp) for a complete reference implementation.

---

## Step 2 - Component Implementation

Run `fprime-util impl` to generate the component implementation files. Implement the required port handlers:

### Outgoing (downlink) data: **`dataIn`**

This port is receiving data from the communication stack and sending it to the hardware for outgoing communications. When done sending the data:

- the input buffer **must** be returned through the `dataReturnOut` port (for memory ownership, see [Design Pattern: Return-To-Sender](../user-manual/framework/memory-management/buffer-pool.md#design-pattern-return-to-sender)) 
- a status **must** be sent via `comStatusOut`. Only a `Fw::Success::SUCCESS` value will trigger the ComQueue to send new outgoing data. See [Communication Adapter Protocol](../reference/communication-adapter-interface.md#communication-adapter-protocol) for more detail.

**Example:**

```cpp
// Note: Code is simplified for clarity. In practice, you may want to implement a
// retry strategy. Refer to Svc/ComStub for a complete reference implementation
void RadioManager::dataIn_handler(const FwIndexType portNum, Fw::Buffer& sendBuffer, const ComCfg::FrameContext& context) {
    // Send data to the driver
    Drv::ByteStreamStatus sendStatus = this->drvSendOut_out(0, sendBuffer);
    // Determine success or failure
    Fw::Success comSuccess = (sendStatus == Drv::ByteStreamStatus::OP_OK) ? Fw::Success::SUCCESS : Fw::Success::FAILURE;
    // [CAUTION: See note below]
    // Return buffer ownership back first
    this->dataReturnOut_out(0, sendBuffer, context);
    // Then send comStatus **after** buffer ownership is returned
    this->comStatusOut_out(0, comSuccess);
}
```

> [!CAUTION]
> The ordering of the last two port calls (`dataReturnOut_out` / `comStatusOut_out`) is important. The first one returns ownership of the buffer back to upstream components such as ComQueue, or ComAggregator if present. The second one signals that we are ready to accept more data to be sent out. However these upstream components may assert if they receive a comStatus but do not own the sent buffer. Users therefore **must** call `dataReturnOut` before calling `comStatusOut`.

### Incoming (uplink) data: **`drvReceiveIn`**

This port is receiving uplink data from the driver (hardware), which is then forwarded to the communications stack through the `dataOut` port. If you are using a different driver, the `drvReceiveIn` port name or strategy may differ.

**Example:**

```cpp
void RadioManager::drvReceiveIn_handler(const FwIndexType portNum,
                                   Fw::Buffer& recvBuffer,
                                   const Drv::ByteStreamStatus& recvStatus) {
    if (recvStatus != Drv::ByteStreamStatus::OP_OK) {
        // Receive failed - return buffer ownership without processing
        this->drvReceiveReturnOut_out(0, recvBuffer);
    } else {
        // Receive successful - forward data with empty context
        ComCfg::FrameContext emptyContext;
        this->dataOut_out(0, recvBuffer, emptyContext);
        // NOTE: we are not returning buffer ownership here. The contract is that
        // once dataOut is done, buffer ownership comes back through dataReturnIn
    }
}
```

### Memory Ownership return port: **`dataReturnIn`**

This port is for receiving ownership back of the buffer that was previously sent on `dataOut`. In our example, we have received that buffer from the driver on `drvReceiveIn`, and we therefore simply return it back to the driver on `drvReceiveReturnOut`.

```cpp
void RadioManager::dataReturnIn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer, const ComCfg::FrameContext& context) {
    this->drvReceiveReturnOut_out(0, fwBuffer);
}
```

### Initiate downlink data flow

As detailed in the [Communication Adapter Protocol](../reference/communication-adapter-interface.md#communication-queue-protocol), the F´ downlink stack expects to receive an initial `Fw::Success::SUCCESS` message via our Com Adapter component's `comStatusOut` port to initiate data flow. This indicates that the radio manager is ready to be handed data to send out for downlink. Projects may implement this as is relevant for their specific radio. 

In our example, we will leverage the ByteStreamDriver's `ready` port, which signals when the driver is ready to receive data.

**Example:**

```cpp
void RadioManager::drvConnected_handler(const FwIndexType portNum) {
    Fw::Success radioSuccess = Fw::Success::SUCCESS;
    this->comStatusOut_out(0, radioSuccess);
}
```

Refer to [`Svc.ComStub` implementation](../../Svc/ComStub/ComStub.cpp) for detailed examples of each handler


---

## Step 3 - Integrate into Topology

As discussed in the Overview, new deployments are by default created with a `Svc.ComStub` implementing the Com Adapter interface. We must replace this with our radio manager instead.

### Remove ComStub

First, we need to take out ComStub of our topology. This is done by using the `ComCcsds.FramingSubtopology` instead of the default `ComCcsds.Subtopology`. Refer to the [ComCcsds definition](../../Svc/Subtopologies/ComCcsds/ComCcsds.fpp) for more detail

```diff
  # ----------------------------------------------------------------------
  # Subtopology imports
  # ----------------------------------------------------------------------
-    import ComCcsds.Subtopology
+    import ComCcsds.FramingSubtopology
```

### Add RadioManager

Next, we will add our RadioManager component to the topology and connect the appropriate ports. We also add the appropriate driver, which is UART in our example.

First, we define the instances:

```python
# in instances.fpp
instance radioManager: MyProject.RadioManager base id <INSERT_BASE_ID>
instance uartDriver: Drv.LinuxUartDriver base id <INSERT_BASE_ID>
```

Next, we use them in our main topology and connect the ports. There are two sets of ports to connect:

- The ports of the RadioManager to the ports of F´ Communication Stack (these components are in the `ComCcsds` subtopology)
- The ports of the RadioManager to the ports of the UART driver

```python
# in topology.fpp
topology MyTopology {
    instance radioManager
    instance uartDriver

    # [ ... more code ...]

    connections RadioManager {
        # Com Adapter Interface ports to F´ Comms stack (in ComCcsds subtopology)
        ComCcsds.framer.dataOut    -> radioManager.dataIn
        radioManager.dataReturnOut -> ComCcsds.framer.dataReturnIn
        radioManager.comStatusOut  -> ComCcsds.framer.comStatusIn
        radioManager.dataOut       -> ComCcsds.frameAccumulator.dataIn
        ComCcsds.frameAccumulator.dataReturnOut -> radioManager.dataReturnIn
        # RadioManager ports to driver (project-defined)
        # If you are using a different driver, these ports may be different
        radioManager.drvSendOut          -> uartDriver.send
        uartDriver.recv                  -> radioManager.drvReceiveIn
        radioManager.drvReceiveReturnOut -> uartDriver.deallocate
        uartDriver.ready                 -> radioManager.drvConnected
    }
}
```

---

## Step 4 - Configure the Driver at Initialization

Configure the underlying driver in your topology initialization code. 

**Example:**

```cpp
void configureTopology() {
    // [ ... other code ...]
        bool open_success = uartDriver.open("/dev/ttyACM0", 
                           Drv::LinuxUartDriver::BAUD_115K, 
                           Drv::LinuxUartDriver::NO_FLOW,
                           Drv::LinuxUartDriver::PARITY_NONE, 
                           1024);
        if (open_success) {
            uartDriver.start(); // configure priority and stack size here if needed
        } else {
            printf("Failed to open UART device\n");
        }
}

void teardownTopology(const TopologyState& state) {
    // [ ... other code ...]
    uartDriver.quitReadThread();
    uartDriver.close();
}
```

---

## Advanced Features

### Asynchronous Transmission

For asynchronous byte stream drivers, use the async send ports and store the context for the callback. See [`Svc.ComStub` async implementation](../../Svc/ComStub/ComStub.cpp) for a complete example.

### Retry Logic

Implement retry logic for transient failures. See [`Svc.ComStub::handleSynchronousSend`](../../Svc/ComStub/ComStub.cpp) for an example with retry limits

---

## Best Practices

- **Follow the protocol**: Review the [Communication Adapter Protocol](../reference/communication-adapter-interface.md#communication-adapter-protocol) requirements carefully.

- **Return buffers promptly**: Return ownership via `*Return*` ports immediately after transmission.

- **Handle errors gracefully**: Emit `Fw::Success::FAILURE` to pause data flow, then `SUCCESS` when recovered.

---

## Additional Resources

- Reference: [Svc.ComStub](../../Svc/ComStub/docs/sdd.md) - Com Adapter implementation passing through data from a ByteStreamDriver
- Reference: [XBeeManager](https://github.com/fprime-community/fprime-sensors/tree/devel/fprime-sensors/XBee/Components/XBeeManager) - Com Adapter implementation for a [XBee radio](https://www.digi.com/products/embedded-systems/digi-xbee/rf-modules)
- [Communication Adapter Interface Reference](../reference/communication-adapter-interface.md) - Complete protocol specification
- [Byte Stream Driver Model](../../Drv/ByteStreamDriverModel/docs/sdd.md) - Driver interface documentation
