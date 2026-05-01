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
    async input port pingIn: Svc.Ping

    @ Ping output port
    output port pingOut: Svc.Ping

    @ Run port for starting packet send cycle
    async input port Run: Svc.Sched

    @ Input configuration port
    async input port configureSectionGroupRate: ConfigureGroupRate

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

    @ Enable / disable telemetry of a group on a section
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
                    $id: FwChanIdType @< The level
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
                      $id: U32 @< The packet ID
                    ) \
      severity activity low \
      id 3 \
      format "Sent packet ID {}"

    @ Couldn't find the packet to send
    event PacketNotFound(
                          $id: U32 @< The packet ID
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
    
    # ----------------------------------------------------------------------
    # Telemetry
    # ----------------------------------------------------------------------

    @ Telemetry send level
    telemetry GroupConfigs: SectionConfigs id 0
    telemetry SectionEnabled: SectionEnabled id 1

    array TelemetrySendSection = [NUM_CONFIGURABLE_TLMPACKETIZER_GROUPS] FwIndexType
    array TelemetrySendPortMap = [TelemetrySection.NUM_SECTIONS] TelemetrySendSection default TELEMETRY_SEND_PORT_MAPPING

  }

}
