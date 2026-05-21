
# Telemetry Collection Functionality

## References

- [F Prime TlmChan SDD](https://github.com/nasa/fprime/blob/devel/Svc/TlmChan/docs/sdd.md)
- [F Prime TlmPacketizer SDD](https://github.com/nasa/fprime/blob/devel/Svc/TlmPacketizer/docs/sdd.md)
- [FPP User Guide — Telemetry](https://nasa.github.io/fpp/fpp-users-guide.html#Defining-Components_Telemetry)

## Overview

Telemetry collection gathers named data values (channels) from components across the system, stores their most recent values, and packages them into packets for downlink to the ground. Components write telemetry values through the framework's telemetry port; these values are collected by a telemetry storage service and periodically sent as packets to the communication stack.

Two telemetry storage implementations are available, each suited to different operational needs.

### Channel-Based Storage (TlmChan)

The channel-based approach stores the latest value of each telemetry channel in an internal table. When triggered by a scheduled port call, it sends all updated channels as individual telemetry packets. Each packet contains a single channel ID, timestamp, and value.

The storage table uses a double-buffered hashing scheme: one buffer is written to by components while the other is read during the send cycle. The buffers swap roles on each cycle. This allows components to continue writing telemetry without being blocked by the send process.

### Packetized Storage (TlmPacketizer)

The packetized approach groups multiple telemetry channels into pre-defined packets. A packet definition table specifies which channels belong to which packets. When telemetry values are written by components, they are placed at the appropriate offset within the packet buffer. On each send cycle, all defined packets are sent with the most recent values.

Packetized telemetry is more bandwidth-efficient than channel-based telemetry because it avoids sending individual channel headers for each value. Packet groups can be individually enabled or disabled via command, allowing operators to select which telemetry sets are active during different operational phases.

The packet definition table also supports different send levels, allowing packets to be grouped by priority or operational phase.

### Telemetry Update Policies

Telemetry channels can be configured with an update policy:

- **Always** — The channel value is included in every downlink cycle regardless of whether it changed.
- **On Change** — The channel value is only included when it differs from the previously reported value.

### Telemetry Limits

Telemetry channels with numeric types can specify limit ranges at three severity levels (red, orange, yellow), each with low and high thresholds. Limit checking is performed by the ground system based on the dictionary definition.
