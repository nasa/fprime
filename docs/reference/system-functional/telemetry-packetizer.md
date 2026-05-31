
# Telemetry Packetizer Functionality

## References

- [F Prime TlmPacketizer SDD](https://github.com/nasa/fprime/blob/devel/Svc/TlmPacketizer/docs/sdd.md)
- [FPP User Guide — Telemetry](https://nasa.github.io/fpp/fpp-users-guide.html#Defining-Components_Telemetry)

## Overview

The Telemetry Packetizer (TlmPacketizer) provides packetized telemetry storage and downlink. It groups multiple telemetry channels into pre-defined packets, sending all channels in a packet together as a single unit. This is more bandwidth-efficient than per-channel telemetry because it avoids repeating individual channel headers for each value. It is suited for deployments with constrained downlink bandwidth or where operators need control over which telemetry sets are active.

### Packet Definitions

A packet definition table specifies which channels belong to which packets. Each packet definition includes:

- A set of channel IDs and their offsets within the packet buffer
- A packet identifier
- A send level that determines when the packet is active

Users specify packet definitions through FPP modeling in the topology where this component is used.

### Channel Storage and Packing

When components write telemetry values, the Packetizer stores them at the appropriate offset within the correct packet buffer. Each packet buffer maintains the most recent values of all its member channels, ready for immediate transmission without additional assembly.

### Downlink Cycle

When triggered by a scheduled port call, the Packetizer sends all defined packets that are currently enabled. Each packet is transmitted as a single communication buffer containing the packed channel values. All channels within a packet are sent together, reducing per-channel overhead.

### Packet Groups and Enabling

Packet groups (sections) can be individually enabled or disabled via command. This allows operators to select which telemetry sets are active during different operational phases. For example:

- A high-rate engineering packet group active during maneuvers
- A low-rate housekeeping group active during cruise
- A diagnostic group enabled only for troubleshooting

### Send Levels

The packet definition table supports different send levels, allowing packets to be grouped by priority or operational phase. Only packets at or below the current send level are transmitted on each cycle.

### Rate Logic

Each telemetry group can be configured with a rate logic mode that controls when packets are sent:

- **SILENCED** — Packet is never sent.
- **EVERY_MAX** — Packet is sent every MAX delta schedule ticks, regardless of whether channels have been updated.
- **ON_CHANGE_MIN** — Packet is sent only when a channel value changes, at a minimum interval of MIN delta schedule ticks.
- **ON_CHANGE_MIN_OR_EVERY_MAX** — Packet is sent on change (at MIN intervals) or at most every MAX intervals, whichever comes first.

The MIN and MAX delta parameters are configured via the CONFIGURE_GROUP_RATES command. If MAX is less than MIN in the combined mode, MAX takes priority.

### Telemetry Limits

As with channel-based telemetry, channels can have limit ranges defined in the dictionary. Limit checking is performed by the ground system.

### Off Nominal

- If a channel update arrives for a channel not in any packet definition, it is silently ignored.
- If the send cycle cannot complete within the scheduling period, packets may be delayed.
