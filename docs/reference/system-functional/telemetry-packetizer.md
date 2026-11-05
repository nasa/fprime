
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

Each group (and each packet override) carries a send state, resolved packet → group → section: **INHERIT_FROM_PARENT** (follow the level above; the default), **SEND_WHEN_SECTION_ENABLED**, **SEND_ALWAYS** (ignore the section enable), or **SEND_NEVER**. Group send states are set with the CONFIGURE_GROUP_SEND command.

Each telemetry group can also be configured with a rate logic mode that controls when packets are sent:

- **EVERY_MAX** — Packet is sent every MAX delta schedule ticks, regardless of whether channels have been updated.
- **ON_CHANGE_MIN** — Packet is sent only when a channel value changes, at a minimum interval of MIN delta schedule ticks.
- **ON_CHANGE_MIN_OR_EVERY_MAX** — Packet is sent on change (at MIN intervals) or at most every MAX intervals, whichever comes first.

The MIN and MAX delta parameters are configured via the CONFIGURE_GROUP_RATES command. If MAX is less than MIN in the combined mode, MAX takes priority.

### Per-Packet Overrides

Beyond the group/section controls above, individual packets can be overridden. An override replaces only the fields it names — the send state, the rates, or both — and the packet keeps following its group for everything else, until the override is cleared:

- **OVERRIDE_PACKET_SEND** — Override a single packet's send state (`SEND_ALWAYS` emits even in a disabled section; `SEND_WHEN_SECTION_ENABLED` still honours it; `SEND_NEVER` silences the packet).
- **OVERRIDE_PACKET_RATES** — Override a single packet's rate logic and MIN/MAX thresholds.
- **GET_PACKET_CONFIG** — Report a packet's stored override and effective configuration as the `PacketConfigReport` event.
- **CLEAR_PACKET_OVERRIDE** — Remove an override, reverting the packet to its group.

Each applied override is reported with the `PacketOverrideSet` event and each clear with `PacketOverrideCleared`. Overrides can be mirrored to, and reloaded from, an external persistence component so they survive reboot. See the [SDD](https://github.com/nasa/fprime/blob/devel/Svc/TlmPacketizer/docs/sdd.md) for the full override, priority, and persistence rules.

### Telemetry Limits

As with channel-based telemetry, channels can have limit ranges defined in the dictionary. Limit checking is performed by the ground system.

### Off Nominal

- If a channel update arrives for a channel not in any packet definition, it is silently ignored.
- If the send cycle cannot complete within the scheduling period, packets may be delayed.
