# A Quick Look at the Hub Pattern

The Hub pattern is a way to distribute F´ applications across some barriers. These barriers may be address-space barriers, platform barriers, or other divides. With the hub pattern, we connect F´ ports through a serialized comm link
and then out the interface on the other side of the barrier. It is built around the hub.

A hub is a component with multiple serialization input and output ports. Typed ports from a calling component are
connected to the Hub's serialization ports. These ports allow any inputs which serialize to the communication bridge
across the divide. On the other side of the divide, the Hub unwraps the calls back into the typed ports. In this way,
typed ports are connected to typed ports using the Hub as an intermediary to get across the divide.

![Hub Pattern](../media/data_model6.png)

**Figure 9. Hub pattern.** Each hub instance is responsible for connecting to a remote node. Input port calls are
repeated to corresponding output ports on a remote hub. These hubs have been demonstrated on Sockets,
ARINC 653 Channels, High-speed hardware buses between nodes, and UARTs between nodes in an embedded system.

## Generic Hub

There is now a standard implementation of the hub pattern. The [GenericHub](../api/c++/html/svc_generic_hub.html) is an
implementation of the hub pattern that passes through F´ ports and `Fw::Buffer`s.