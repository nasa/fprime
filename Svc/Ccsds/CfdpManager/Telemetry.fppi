@ CFDP Manager Telemetry
@
@ This file defines proposed telemetry channels for CfdpManager based on
@ telemetry counters from the NASA CF (CFDP) Application.
@
@ Note: These are currently PROPOSALS and not yet implemented.
@ Implementation would require adding telemetry channels and updating
@ the code to emit telemetry at appropriate locations.

@ Array of per-channel telemetry counters
@ Each element contains receive counters, sent counters, fault counters,
@ queue depths, and activity counters for one CFDP channel.
telemetry ChannelTelemetry: Cfdp.ChannelTelemetryArray
