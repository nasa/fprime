# Implement a Framing Protocol

This How-To Guide provides a step-by-step guide to implementing a custom framing protocol in F Prime Flight Software

The default [F Prime Protocol](../../FprimeProtocol/docs/sdd.md) is a lightweight protocol used to get development started quickly with a low-overhead communications protocol that the [F Prime GDS](https://github.com/nasa/fprime-gds) understands. However, as projects mature, there may be a need to implement a framing protocol that fits mission requirements. This document provides an overview of how to implement a custom framing protocol in F Prime Flight Software, and how to integrate it with the F Prime GDS.

## Overview

This guide demonstrates how to implement a custom framing protocol, referred to here as **MyCustomProtocol**. The protocol defines how data is wrapped (framed) for transmission and how frames are validated and unpacked (deframed) on reception. 

A reference implementation of a custom framing protocol is available in the `fprime-examples`:
- [C++ CustomFraming Example](https://github.com/nasa/fprime-examples/tree/devel/FlightExamples/CustomFraming)
- [GDS Plugin Example](https://github.com/nasa/fprime-examples/tree/devel/GdsExamples/gds-plugins/src/framing)

This example implements a custom protocol called **Decaf**, which is a simple protocol with a start word, length field, and CRC32 checksum.

This guide is divided into two main sections: flight software implementation and GDS integration. Note that if you are aiming to integrate with another GDS and do not wish to use the F´ GDS, you can skip the GDS section.

## Flight Software Implementation

To implement a custom framing protocol in F´, will need to implement the following:
- **Framer**: A component that wraps payload data into frames for transmission.
- **Deframer**: A component that unpacks received frames, extracts the payload data, and validate the frame.
- **FrameDetector** (optional): A helper class that detects the start and end of frames in a stream of data, if your transport is stream-based (e.g., TCP, UART).

The following examples will walk through the implementation of a custom framer and deframer for a hypothetical **MyCustomProtocol** protocol. Implementation details are left to the reader, but examples of such implementations can be found in the [fprime-examples repository](https://github.com/nasa/fprime-examples/tree/devel/FlightExamples/CustomFraming), or within the F´ codebase itself ([Svc.FprimeFramer](../../Svc/FprimeFramer/docs/sdd.md) and [Svc.FprimeDeframer](../../Svc/FprimeDeframer/docs/sdd.md)).

1. **Define the Framer and Deframer FPP Components**

   Framer and Deframer components should implement the FPP interfaces by including them.

   In  `MyCustomFramer.fpp`:
   ```
    passive component MyCustomFramer {
        include "path/to/fprime/Svc/Interfaces/FramerInterface.fppi"
        [...]
    }
   ```
   And in `MyCustomDeframer.fpp`:
   ```
    @ Deframer implementation for MyCustomProtocol
    passive component MyCustomDeframer {
        include "path/to/fprime/Svc/Interfaces/DeframerInterface.fppi"
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

4. **(Optional) Implement a Frame Detector**

   In some configurations, your communications manager component may produce full frames on reception. This may be the case with some radios that have frame synchronization built-in, or if delimiting frames is handled by another subsystem. 
   However, if your underlying data transport is stream-based (e.g. TCP, UART) or if you can't guarantee frames will always be received in full by your communications manager, you will need to implement a mechanism to delimit frames. F Prime provides this capability with the [Svc.FrameAccumulator](../../Svc/FrameAccumulator/docs/sdd.md) component, which accumulates a data stream into a circular buffer, and uses a helper `FrameDetector` to detect when a frame is present.
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

To support your custom protocol in the F´ GDS, implement a GDS framing plugin. For example, in Python:

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
```

Make sure to [package and install the plugin in your virtual environment](https://fprime.jpl.nasa.gov/devel/docs/how-to/develop-gds-plugins/#packaging-and-testing-plugins) for the GDS to be able to load it, then run it:

```
fprime-gds --framing-selection MyCustomFramerDeframer
```

## References 

- [C++ CustomFraming Example](https://github.com/nasa/fprime-examples/tree/devel/FlightExamples/CustomFraming)
- [GDS Plugin Example](https://github.com/nasa/fprime-examples/tree/devel/GdsExamples/gds-plugins/src/framing)
- [F Prime GDS Framing Plugin](../reference/gds-plugins/framing.md)
- [F Prime Communication Adapter Interface](../reference/communication-adapter-interface.md)
- [F Prime GDS Plugin Development](https://fprime.jpl.nasa.gov/devel/docs/how-to/develop-gds-plugins/)

