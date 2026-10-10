module Svc {
  @ A component for storing telemetry
  active component TlmPacketizer {
    # ----------------------------------------------------------------------
    # Types
    # ----------------------------------------------------------------------
    
    struct GroupConfig {
      enabled: Fw.Enabled       @< Enable / Disable Telemetry Output
      forceEnabled: Fw.Enabled  @< Force Enable / Disable Telemetry Output
      rateLogic: RateLogic      @< Rate Logic Configuration
      min: U32                  @< Minimum Sched Ticks when in ON_CHANGE_MIN
      max: U32                  @< Maximum Sched Ticks when in EVERY_MAX
    }

    array GroupConfigs = [NUM_CONFIGURABLE_TLMPACKETIZER_GROUPS] GroupConfig
    array SectionConfigs = [TelemetrySection.NUM_SECTIONS] GroupConfigs default TELEMETRY_SECTION_DEFAULTS
    array SectionEnabled = [TelemetrySection.NUM_SECTIONS] Fw.Enabled default TELEMETRY_SECTION_ENABLED_DEFAULTS

    # ----------------------------------------------------------------------
    # General ports
    # ----------------------------------------------------------------------

    @ Packet send port
    @ Ordered by Section, Group
    output port PktSend: [TELEMETRY_SEND_PORTS] Fw.Com

    async input port controlIn: EnableSection

    @ Ping input port
    async input port pingIn: Svc.Ping drop

    @ Ping output port
    output port pingOut: Svc.Ping

    @ Run port for starting packet send cycle
    async input port Run: Svc.Sched

    @ Input configuration port
    async input port configureSectionGroupRate: ConfigureGroupRate

    @ Per-packet configuration mirror to persistent storage managed by an external component
    @ Each entry carries the full override for the addressed packet/section and is pushed
    @ whenever an ENABLE_PACKET / FORCE_PACKET / CONFIGURE_PACKET_RATES command changes it.
    output port configOut: TlmPacketConfigUpdate

    @ Per-packet configuration reload from the persistence storage component
    @ That storage component pushes it's config into this port when commanded to do so (usually at boot-up)
    async input port configIn: TlmPacketConfigUpdate

    @ Telemetry input port
    sync input port TlmRecv: Fw.Tlm

    @ Telemetry getter port
    sync input port TlmGet: Fw.TlmGet

    # ----------------------------------------------------------------------
    # Special ports
    # ----------------------------------------------------------------------

    @ Command receive
    command recv port cmdIn

    @ Command registration
    command reg port cmdRegOut

    @ Command response
    command resp port cmdResponseOut

    @ Event
    event port eventOut

    @ Telemetry
    telemetry port tlmOut

    @ Text event
    text event port textEventOut

    @ Time get
    time get port timeGetOut
    
    @ Parameter get port
    param get port paramGetOut

    @ Parameter set port
    param set port paramSetOut

    # ----------------------------------------------------------------------
    # Commands
    # ----------------------------------------------------------------------

    @ Set telemetry send level
    async command SET_LEVEL(
                             level: FwChanIdType @< The I32 command argument
                           ) \
      opcode 0

    @ Force a packet to be sent
    async command SEND_PKT(
                            $id: U32                    @< The packet ID
                            section: TelemetrySection   @< Section to emit packet
                          ) \
      opcode 1

    @ Enable / disable a telemetry section
    async command ENABLE_SECTION(
                                section: TelemetrySection   @< Section grouping to configure
                                enable: Fw.Enabled          @< Section enabled or disabled
                              ) \
      opcode 2

    @ Enable / disable telemetry of a group on a section
    async command ENABLE_GROUP(
                                section: TelemetrySection   @< Section grouping to configure
                                tlmGroup: FwChanIdType      @< Group Identifier
                                enable: Fw.Enabled          @< Section enabled or disabled
                              ) \
      opcode 3
    
    @ Force telemetering a group on a section, even if disabled
    async command FORCE_GROUP(
                                    section: TelemetrySection   @< Section grouping
                                    tlmGroup: FwChanIdType      @< Group Identifier
                                    enable: Fw.Enabled          @< Section enabled or disabled
                                  ) \
      opcode 4

    @ Set Min and Max Deltas between successive packets
    async command CONFIGURE_GROUP_RATES(
                                        section: TelemetrySection   @< Section grouping
                                        tlmGroup: FwChanIdType      @< Group Identifier
                                        rateLogic: RateLogic        @< Rate Logic
                                        minDelta: U32               @< Minimum Sched Ticks to send packets on updates when using ON_CHANGE logic
                                        maxDelta: U32               @< Maximum Sched Ticks between packets to send when using EVERY_MAX logic
                                      ) \
      opcode 5

    @ Query the effective per-packet configuration. The result is emitted on the
    @ QueriedPacketConfig telemetry channel; unknown ids raise UnknownPacketId.
    async command GET_PACKET_CONFIG(
                                    packetId: U32               @< Packet identifier
                                    section: TelemetrySection   @< Section to query
                                  ) \
      opcode 6

    @ Enable / disable a single packet in a section (per-packet override)
    async command ENABLE_PACKET(
                                 packetId: U32               @< Packet identifier
                                 section: TelemetrySection   @< Section to configure
                                 enable: Fw.Enabled          @< Enable / disable this packet
                               ) \
      opcode 7

    @ Force telemeter a single packet even when it (or its section) is disabled
    async command FORCE_PACKET(
                                packetId: U32               @< Packet identifier
                                section: TelemetrySection   @< Section to configure
                                enable: Fw.Enabled          @< Force enable / disable
                              ) \
      opcode 8

    @ Configure the rate logic and thresholds for a single packet
    async command CONFIGURE_PACKET_RATES(
                                          packetId: U32               @< Packet identifier
                                          section: TelemetrySection   @< Section to configure
                                          rateLogic: RateLogic        @< Rate logic
                                          minDelta: U32               @< Minimum Sched ticks between sends (ON_CHANGE_MIN logic)
                                          maxDelta: U32               @< Maximum Sched ticks between sends (EVERY_MAX logic)
                                        ) \
      opcode 9

    @ Clear a single packet's per-packet override, reverting it to group-derived behavior.
    @ The cleared state is mirrored out configOut so persistent storage stays consistent.
    async command CLEAR_PACKET_OVERRIDE(
                                         packetId: U32               @< Packet identifier
                                         section: TelemetrySection   @< Section to clear
                                       ) \
      opcode 10

    @ Parameter to control section enable flags
    external param SECTION_ENABLED: SectionEnabled default TELEMETRY_SECTION_ENABLED_DEFAULTS
    @ Parameter to control section configuration
    external param SECTION_CONFIGS: SectionConfigs default TELEMETRY_SECTION_DEFAULTS

    # ----------------------------------------------------------------------
    # Events
    # ----------------------------------------------------------------------

    @ Telemetry channel is not part of a telemetry packet.
    event NoChan(
                  Id: FwChanIdType @< The telemetry ID
                ) \
      severity warning low \
      id 0 \
      format "Telemetry ID 0x{x} not packetized"

    @ Telemetry send level set
    event LevelSet(
                    level: FwChanIdType @< The level
                  ) \
      severity activity high \
      id 1 \
      format "Telemetry send level to {}"

    @ Telemetry send level set
    event MaxLevelExceed(
                          level: FwChanIdType @< The level
                          max: FwChanIdType @< The max packet level
                        ) \
      severity warning low \
      id 2 \
      format "Requested send level {} higher than max packet level of {}"

    @ Packet manually sent
    event PacketSent(
                      packetId: U32 @< The packet ID
                    ) \
      severity activity low \
      id 3 \
      format "Sent packet ID {}"

    @ Couldn't find the packet to send
    event PacketNotFound(
                          packetId: U32 @< The packet ID
                        ) \
      severity warning low \
      id 4 \
      format "Could not find packet ID {}"

    event SectionUnconfigurable(
                                section: TelemetrySection @< The Section
                                enable: Fw.Enabled        @< Attempted Configuration
                               ) \
      severity warning low \
      id 5 \
      format "Section {} is unconfigurable and cannot be set to {}"

    @ Telemetry value larger than the configured channel size
    event OversizedChannel(
                            Id: FwChanIdType @< The telemetry ID
                            valSize: FwSizeType @< The received value size
                            expected: FwSizeType @< The configured channel size
                          ) \
      severity warning high \
      id 6 \
      format "Telemetry ID 0x{x} update of size {} exceeds configured size {}" \
      throttle 10

    @ A configuration command or query referenced a packet id not present in this deployment
    event UnknownPacketId(
                          packetId: U32 @< The packet id
                        ) \
      severity warning low \
      id 7 \
      format "Packet id {} not found in packet list"

    @ A configIn batch carried more entries than the packetizer can accept
    event ConfigBatchTruncated(
                                count: FwSizeType @< The number of entries offered
                                cap: U32          @< The maximum number of entries accepted
                              ) \
      severity warning high \
      id 8 \
      format "configIn batch of {} entries exceeds cap {}; extra entries dropped" \
      throttle 10

    # ----------------------------------------------------------------------
    # Telemetry
    # ----------------------------------------------------------------------

    @ Telemetry send level
    telemetry GroupConfigs: SectionConfigs id 0
    telemetry SectionEnabled: SectionEnabled id 1

    @ Effective per-packet configuration, emitted in response to GET_PACKET_CONFIG
    telemetry QueriedPacketConfig: PacketConfigEntry id 2

    array TelemetrySendSection = [NUM_CONFIGURABLE_TLMPACKETIZER_GROUPS] FwIndexType
    array TelemetrySendPortMap = [TelemetrySection.NUM_SECTIONS] TelemetrySendSection default TELEMETRY_SEND_PORT_MAPPING

  }

}
