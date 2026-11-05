
# Telemetry Channel Storage Functionality

## References

- [F Prime TlmChan SDD](https://github.com/nasa/fprime/blob/devel/Svc/TlmChan/docs/sdd.md)
- [FPP User Guide — Telemetry](https://nasa.github.io/fpp/fpp-users-guide.html#Defining-Components_Telemetry)

## Overview

The Telemetry Channel component (TlmChan) provides channel-based telemetry storage and downlink. It stores the latest value of each telemetry channel written by components across the system and sends them individually on each downlink cycle. This is the simpler of the two telemetry implementations in F Prime and is suitable for deployments where per-channel granularity is acceptable and bandwidth is not tightly constrained.

### Channel Storage

Components write telemetry values through the framework's telemetry port. Each value includes a channel ID, a typed value, and a timestamp. The TlmChan component stores the most recent value for each channel in an internal lookup table.

The storage table uses a double-buffered hashing scheme: one buffer is written to by components while the other is read during the send cycle. The buffers swap roles on each cycle. This allows components to continue writing telemetry without being blocked by the send process.

### Downlink Cycle

When triggered by a scheduled port call (typically from a rate group), TlmChan iterates over its stored channels and sends each updated value as an individual telemetry packet. Each packet contains a single channel ID, timestamp, and serialized value. The packets are sent to the communication stack for transmission to the ground.

### Telemetry Update Policies

Telemetry channels can be configured with an update policy:

- **Always** — The channel value is included in every downlink cycle regardless of whether it changed.
- **On Change** — The channel value is only included when it differs from the previously reported value.

### Telemetry Limits

Telemetry channels with numeric types can specify limit ranges at three severity levels (red, orange, yellow), each with low and high thresholds. Limit checking is performed by the ground system based on the dictionary definition.

### Off Nominal

- If the channel table is full, additional channels cannot be stored and a warning is issued.
- If the send cycle takes longer than the scheduling period, channels may be stale by the time they are transmitted.
