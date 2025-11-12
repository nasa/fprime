# Implement a Framing Protocol

This How-To Guide provides a step-by-step guide to implementing a custom framing protocol in F Prime Flight Software.

Modern F´ deployments use the CCSDS protocol by default via the `Svc.ComCcsds` subtopology. The lightweight [F Prime Protocol](../../Svc/FprimeProtocol/docs/sdd.md) (available via `Svc.ComFprime`) is also available as an alternative low-overhead communications protocol that the [F Prime GDS](https://github.com/nasa/fprime-gds) understands. However, some projects choose to implement another framing protocol that fits their mission requirements. This document provides an overview of how to implement a custom framing protocol in F Prime Flight Software, and how to integrate it with the F Prime GDS.

## Overview

This guide demonstrates how to implement a custom framing protocol, referred to here as **MyCustomProtocol**. The protocol defines how data is wrapped (framed) for transmission and how frames are validated and unpacked (deframed) on reception. For a bi-directional framing implementation (uplink and downlink), you will need to implement both a framer and a deframer component. A framer is required for downlink (flight software → GDS). A deframer (and optionally a FrameDetector) is needed for deframing uplink messages (GDS → flight software).

A reference implementation of a custom framing protocol (the "Decaf Protocol") is available in the `fprime-examples` repository:
- [C++ CustomFraming Example](https://github.com/nasa/fprime-examples/tree/devel/FlightExamples/CustomFraming)
- [GDS Plugin Example](https://github.com/nasa/fprime-examples/tree/devel/GdsExamples/gds-plugins/src/framing)

This guide is divided into two main sections: flight software implementation and GDS integration. If you are aiming to integrate with another GDS and do not wish to use the F´ GDS, you can skip the GDS section.

> [!NOTE]
> When using the reference examples, it is recommended to check out the `fprime-examples` repository **at the same release tag** as your F´ core installation to ensure compatibility. You can find which version of F´ you are using with `fprime-util version-check`.

## Prerequisites: Understanding Subtopologies

**Important**: Before implementing custom framing protocols, you should familiarize yourself with F´ subtopologies. Modern F´ deployments ship with the `Svc.ComCcsds` subtopology by default, which provides a standard communication stack including framing/deframing components.

To implement custom framing protocols, you will typically need to:
1. **Understand the existing communication subtopology**: Review how the default `Svc.ComCcsds` subtopology is structured and integrated into your deployment
2. **Manually import components**: Copy the relevant topology code from the subtopology into your main topology so you can modify the component connections as needed, and remove the old `import Svc.ComCcsds` statement.
3. **Replace standard components**: Substitute the default framer/deframer components with your custom implementations

For more information on working with subtopologies, see the [Subtopologies Guide](../user-manual/design-patterns/subtopologies.md). This understanding is essential before proceeding with custom framing implementation.

## Flight Software Implementation

To implement a custom framing protocol in F´, will need to implement the following:
- **Framer**: An F´ component that wraps payload data into frames for transmission.
- **Deframer**: An F´ component that unpacks received frames, extracts the payload data, and validates the frame.
- **FrameDetector** Helper Class (optional): A C++ helper class (not an F´ component) that detects the start and end of frames in a stream of data, if your transport does not guarantee complete packets (e.g., TCP, UART).

> [!TIP]
> When creating framer/deframer components using `fprime-util new --component`, the recommended settings are to use passive components with events enabled.

The following examples will walk through the implementation of a custom framer and deframer for a hypothetical **MyCustomProtocol** protocol. Implementation details are left to the reader, but examples of such implementations can be found in the [fprime-examples repository](https://github.com/nasa/fprime-examples/tree/devel/FlightExamples/CustomFraming), or within the F´ codebase itself ([Svc.FprimeFramer](../../Svc/FprimeFramer/docs/sdd.md) and [Svc.FprimeDeframer](../../Svc/FprimeDeframer/docs/sdd.md)).

### Pitfalls and Best Practices

Before implementing, consider these best practices:

1. **Centralize Protocol Constants**: Define protocol constants (e.g., start word, header structure, field sizes) in a dedicated `.fpp` types file (e.g., `Types/Types.fpp`). This allows constants like start words, field types, or spacecraft IDs to be reused consistently across your framer, deframer, and frame detector.

2. **Endianness Convention**: F´ serializes all integer types in big-endian by default (network byte order), as defined in `Fw::Serializable`. If your protocol requires little-endian fields, you must specify so during serialization with the `Fw::Endianness::LITTLE` mode, or manually order bytes as needed. Payload serialization and deserialization target the `Fw::SerialBufferBase` interface, so user-defined serializable types can use `serializeTo`/`deserializeFrom` with a compatible buffer.

3. **Set the APID in the Deframer**: Your deframer **must** extract and set the APID (Application ID) in the `FrameContext` before emitting packets downstream via `dataOut`. Without this, the `Svc.FprimeRouter` will not know where to route packets to. This is a critical requirement, unless you are also implementing a custom router and define your own requirements (not recommended and outside the scope of this guide).

4. **Payload vs. Frame Structure**: A framing protocol should only define the outer frame structure (headers and trailers). The internal payload structure is determined by the upper-layer protocols and applications that produce and consume the data, not by the framing protocol itself.

### Implementation Steps

1. **Define the Framer and Deframer FPP Components**

   Framer and Deframer components should implement the FPP interfaces by including them.

   In  `MyCustomFramer.fpp`:
   ```
    passive component MyCustomFramer {
        import Svc.Framer
        [...]
    }
   ```
   And in `MyCustomDeframer.fpp`:
   ```
    @ Deframer implementation for MyCustomProtocol
    passive component MyCustomDeframer {
        import Svc.Deframer
        [...]
    }
   ```

2. **Implement the Framer C++ Component**

   Implement the required handler functions:
   ```cpp
   // ...existing code...
   void MyCustomFramer ::dataIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) {
       // TODO: Implement framing logic
   }

   void MyCustomFramer ::comStatusIn_handler(FwIndexType portNum, Fw::Success& condition) {
        this->comStatusOut_out(portNum, condition); // pass comStatus through (unless project requires otherwise)
   }

   void MyCustomFramer ::dataReturnIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) {
        // TODO: handle return of data ownership
        // For example, if component required to allocate from a buffer manager, return the buffer to the manager
   }
   // ...existing code...
   ```

3. **Implement the Deframer C++ Component**

   Similarly, implement the required handler functions:
   ```cpp
   // ...existing code...
   void MyCustomDeframer ::dataIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) {
       // TODO: Implement deframing logic
   }

   void MyCustomDeframer ::dataReturnIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) {
        // TODO: handle return of data ownership
   }
   // ...existing code...
   ```

4. **Integrate Components into Your Topology**

    After implementing the framer, deframer, and optionally the frame detector, integrate these components into your main topology. This typically involves:
    - Removing the import statement for the existing communication subtopology (e.g., `import Svc.ComCcsds`).
    - Manually adding the topology code from the subtopology as needed. See [ComFprime.fpp](https://github.com/nasa/fprime/blob/devel/Svc/Subtopologies/ComFprime/ComFprime.fpp) for a reference code to add to your topology. You should be copy-pasting most of this code into your own topology, and adapt the C++ phase code as necessary.
    - Updating the `framer` and `deframer` instances to use your custom implementations.
    
    Example snippet from `Top/Topology.fpp`:
    ```fpp
    // Remove or comment out the existing subtopology import
    // import Svc.ComCcsds

    // Manually add topology code from the subtopology as needed
    // ...

    // Replace instances of custom framer and deframer
    instance framer: MyCustomFramer base id 0x1000
    instance deframer: MyCustomDeframer base id 0x2000
    ```
    
    **Troubleshooting Common Integration Issues**:
    
    After removing the CCSDS subtopology and adding your custom framing components, you may encounter compilation errors:
    
    - **Missing header errors** (e.g., `fatal error: Svc/Subtopologies/ComCcsds/ComCcsdsConfig/FppConstantsAc.hpp: No such file or directory`):
      - Remove all mentions of `ComCcsds` from your auto-generated `<DeploymentName>TopologyDefs.hpp` file (located in `<deployment>/Top/`).
      - After significant topology changes, you may need to do a clean rebuild with `fprime-util generate --force`
    
    - **Undeclared port enum errors** (e.g., `error: 'Ports_ComPacketQueue' has not been declared`):
      - Include the generated port headers in your `<DeploymentName>TopologyDefs.hpp`:
        ```cpp
        #include <MyDeployment/Top/Ports_ComPacketQueueEnumAc.hpp>
        #include <MyDeployment/Top/Ports_ComBufferQueueEnumAc.hpp>
        ```
    
    - **Linker errors** (e.g., `undefined reference to 'vtable for CustomFraming::MyCustomFrameDetector'`):
      - Ensure your custom Detector is added as a dependency of your Deframer or deployment, using the `DEPENDS` keyword. In your Deframer's `CMakeLists.txt`, add:
        ```cmake
        register_fprime_module(
            [...]
            DEPENDS
                CustomFraming_DecafFrameDetector  # This is a helper module and cannot be resolved by FPP dependencies alone
        )
        ```

5. **(Optional) Implement a Frame Detector**

   _When is this not needed?_  
   If your communications manager component always receives complete frames, you do not need to implement frame detection. This can be the case when using:
   - Radios with built-in frame synchronization
   - Message-oriented transport-layer protocols where frame boundaries are guaranteed (e.g. UDP)

   _When is this needed?_  
   If your data transport is stream-based and does not preserve message boundaries, you must implement a mechanism to delimit frames. Examples include:
   - TCP connections (stream-based, no inherent message boundaries)
   - UART/serial connections (e.g. UART radios such as XBee)
   
   F Prime provides this capability with the [Svc.FrameAccumulator](../../Svc/FrameAccumulator/docs/sdd.md) component, which uses a circular buffer and a helper `FrameDetector` to identify complete frames in the data stream.

   > [!NOTE]
   > The `FrameDetector` is a C++ helper class, not an FPP component. It does not have an `.fpp` definition and is used internally by `Svc.FrameAccumulator`.

   To use the `Svc.FrameAccumulator`, you need to configure it with a FrameDetector that detects when a frame is present:
   **MyCustomFrameDetector.hpp**
   ```cpp
    #include <Svc/FrameDetector/FrameDetector.hpp>

    class MyCustomFrameDetector : public Svc::FrameDetector {
      public:
        Svc::FrameDetector::Status detect(const Types::CircularBuffer& data, FwSizeType& size_out) const override;
    };
   ```

   **MyCustomFrameDetector.cpp**
   ```cpp
   // ...existing code...
   Svc::FrameDetector::Status MyCustomFrameDetector::detect(const Types::CircularBuffer& data, FwSizeType& size_out) const {
       // TODO: Implement frame boundary detection
       // This can include searching for start words, validating headers, checking lengths, checking CRC and hashes, etc.
       // Utilities exist for CRC under Utils/Hash, and examples are shown in Svc/Ccsds/Utils or in fprime-examples repo
       // Refer to the Svc.FrameDetector documentation for details on how to implement this
       return Svc::FrameDetector::NO_FRAME_DETECTED;
   }
   // ...existing code...
   ```

   Then configure the `Svc.FrameAccumulator` component to use your custom frame detector in your Topology CPP:
   **Top/Topology.cpp**
   ```cpp
    #include <path/to/MyCustomFrameDetector.hpp>
    // ...existing code...
    MyCustomFrameDetector frameDetector;
    // ...existing code...
    frameAccumulator.configure(frameDetector, 1, mallocator, 2048);
    ```


## F´ GDS Implementation

To support your custom protocol in the F´ GDS, implement a GDS framing plugin. The GDS plugin system allows you to customize GDS behavior with user-provided code. For new framing protocols, you will need to implement a plugin that extends the `FramerDeframer`. This is further documented in the [How-To Develop a GDS Plugin Guide](./develop-gds-plugins.md) and [F Prime GDS Framing Plugin reference](../reference/gds-plugins/framing.md).

For example, in Python:

```python
from fprime_gds.common.communication.framing import FramerDeframer
from fprime_gds.plugin.definitions import gds_plugin

@gds_plugin(FramerDeframer)
class MyCustomFramerDeframer(FramerDeframer):
    """GDS plugin for MyCustomProtocol framing"""
    def frame(self, data):
        # TODO: Implement framing logic
        return frame

    def deframe(self, data, no_copy=False):
        # TODO: Implement deframing logic
        return packet, leftover_data, discarded_data

    def get_name(self):
        # TODO: Return the protocol name for selection with `fprime-gds --framing-selection <selection>`
        return "MyCustomProtocol"
```

Make sure to [package and install the plugin in your virtual environment](./develop-gds-plugins.md#packaging-and-testing-plugins) for the GDS to be able to load it, then run it:

```
fprime-gds --framing-selection MyCustomProtocol
```

## References 

- [C++ CustomFraming Example](https://github.com/nasa/fprime-examples/tree/devel/FlightExamples/CustomFraming)
- [GDS Plugin Example](https://github.com/nasa/fprime-examples/tree/devel/GdsExamples/gds-plugins/src/framing)
- [F Prime GDS Framing Plugin](../reference/gds-plugins/framing.md)
- [F Prime Communication Adapter Interface](../reference/communication-adapter-interface.md)
- [F Prime GDS Plugin Development](https://fprime.jpl.nasa.gov/devel/docs/how-to/develop-gds-plugins/)

