# ======================================================================
# TlmPacketizerCfg.fpp
# Constants and types for configuring TlmPacketizer (sections, groups, send ports, packet offset type)
# ======================================================================
@ The type of an offset within a telemetry packet buffer.
@ Deliberately defined here (global scope) rather than in FpConfig.fpp so that
@ projects which override FpConfig.fpp continue to build without adding it.
type FwTlmPacketizeOffsetType = U16

module Svc {
    @ Configurable enum supporting multiple resampled telemetry sections
    @ Each section creates output indices for each telemetry group
    enum TelemetrySection {
        # REALTIME is a project-selected name. Default configuration uses REALTIME as this is a common section
        REALTIME, @< OPTIONAL: realtime telemetry downlink through communication stack
        # RECORDED is a project-selected name. Default configuration uses RECORDED as this is a common section
        RECORDED, @< OPTIONAL: recorded telemetry stored on disk for later retrieval
        NUM_SECTIONS,     @< REQUIRED: Counter, leave as last element.
    }

    @ Maximum number of packets the packetizer can be configured with
    constant MAX_PACKETIZER_PACKETS = 50

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
    ]

    @ Default configuration for section ENABLED states. In the default configuration, all sections start ENABLED.
    constant TELEMETRY_SECTION_ENABLED_DEFAULTS = [Fw.Enabled.ENABLED, Fw.Enabled.ENABLED]

    @ Default Group configuration. The group follows its section's enable (INHERIT_FROM_PARENT) and uses output-on-change with no min or max time thresholds.
    constant DEFAULT_GROUP_CONFIG = { sendState = TlmSendState.INHERIT_FROM_PARENT, rates = { rateLogic = RateLogic.ON_CHANGE_MIN, min = 0, max = 0 } }

    @ Sets the default configuration for all sections and groups. Each section/group can be overridden. This acts as
    @ the default value for the SECTION_CONFIGS parameter. Thus runtime commands and parameter updates chan change this
    @ value for a running system. If the system ships no parameter database, these settings will be used.
    @
    @ Note: here DEFAULT_GROUP_CONFIG is used to configure all sections/groups. However, users may inline literals instead.
    constant TELEMETRY_SECTION_DEFAULTS = [
        # REALTIME section
        [
            # Group 0
            DEFAULT_GROUP_CONFIG,
            # Group 1
            DEFAULT_GROUP_CONFIG,
            # Group 2
            DEFAULT_GROUP_CONFIG,
            # Group 3
            DEFAULT_GROUP_CONFIG,
        ],
        # RECORDED section
        [
            # Group 0
            DEFAULT_GROUP_CONFIG,
            # Group 1
            DEFAULT_GROUP_CONFIG,
            # Group 2
            DEFAULT_GROUP_CONFIG,
            # Group 3
            DEFAULT_GROUP_CONFIG,
        ]
    ]
}
