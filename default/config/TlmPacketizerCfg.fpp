# ======================================================================
# TlmPacketizerCfg.fpp
# Constants for configuring TlmPacketizer Send Logic Levels
# ======================================================================
module Svc {
    @ Configurable enum supporting multiple resampled telemetry sections
    @ Each section creates output indices for each telemetry group
    enum TelemetrySection {
        PRIMARY,
        SECONDARY,
        TERTIARY,
        NUM_SECTIONS,     @< REQUIRED: Counter, leave as last element.
    }

    @ Greatest packet group
    constant MAX_CONFIGURABLE_TLMPACKETIZER_GROUP = 3

    constant NUM_CONFIGURABLE_TLMPACKETIZER_GROUPS = MAX_CONFIGURABLE_TLMPACKETIZER_GROUP + 1

    @ Number of telemetry output (send) ports. These ports are all the outputs. Each section/group uses the configuration
    @ TELEMETRY_SEND_PORTS to determine which port to send based on the group and section.
    @
    @ This default configuration uses one port per section
    constant TELEMETRY_SEND_PORTS = TelemetrySection.NUM_SECTIONS

    @ Telemetry Output Ports - 2D array of [Section][Group] FwIndexType indicating which port to send telemetry on for a given
    @ section/group. The values must be greater than or equal to 0 and less than TELEMETRY_SEND_PORTS.
    @
    @ This default configuration sends all telemetry on port 0 restoring the original TlmPacketizer behavior.
    constant TELEMETRY_SEND_PORT_MAPPING = [
        [0, 0, 0, 0],
        [1, 1, 1, 1],
        [2, 2, 2, 2]
    ]
}
