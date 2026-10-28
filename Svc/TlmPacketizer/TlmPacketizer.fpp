module Svc {
  @ Rate policy
  struct RateConfig {
    rateLogic: RateLogic  @< Rate logic configuration
    min: U32              @< Minimum Sched ticks between sends when using ON_CHANGE_MIN logic
    max: U32              @< Maximum Sched ticks between sends when using EVERY_MAX logic
  }

  @ Telemetry send policy (send state + rates), shared by the per-group config and per-packet override.
  struct TlmSendConfig {
    sendState: TlmSendState  @< Send state
    rates: RateConfig   @< Rate policy
  }

  @ Which fields of a packet's config override its group
  enum PacketOverrideType : U8 {
    GROUP_DEFAULT            @< No override: the packet follows its group
    OVERRIDE_SEND            @< The send state is overridden; rates follow the group
    OVERRIDE_RATES           @< The rates are overridden; the send state follows the group
    OVERRIDE_SEND_AND_RATES  @< Both the send state and the rates are overridden
  }

  @ Per-packet override: which fields apply and their values
  struct PacketOverrideConfig {
    overrideType: PacketOverrideType  @< Fields of sendConfig that override the group
    sendConfig: TlmSendConfig         @< Override values (only the fields selected by overrideType apply)
  }

  @ A single per-packet override record: mirrored on packetOverrideOut, reloaded from packetOverrideIn, reported by GET_PACKET_CONFIG
  struct PacketConfigEntry {
    packetId: U32                      @< Packet identifier
    section: TelemetrySection          @< Section the override applies to
    override: PacketOverrideConfig     @< The override
  }

  @ A component for storing telemetry
  active component TlmPacketizer {
    # ----------------------------------------------------------------------
    # Types
    # ----------------------------------------------------------------------
    
    array GroupConfigs = [NUM_CONFIGURABLE_TLMPACKETIZER_GROUPS] TlmSendConfig
    array SectionConfigs = [TelemetrySection.NUM_SECTIONS] GroupConfigs default TELEMETRY_SECTION_DEFAULTS
    array SectionEnabled = [TelemetrySection.NUM_SECTIONS] Fw.Enabled default TELEMETRY_SECTION_ENABLED_DEFAULTS
    array PacketOverrides = [MAX_PACKETIZER_PACKETS] PacketOverrideConfig
    array SectionPacketOverrides = [TelemetrySection.NUM_SECTIONS] PacketOverrides

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

    @ Per-packet override mirror to an external persistence component
    output port packetOverrideOut: Fw.BufferSend

    @ Allocate the buffers sent on packetOverrideOut and deallocate them when they come back on packetOverrideReturnIn
    import Svc.BufferAllocation

    @ Return of a packetOverrideOut mirror buffer from the persistence component; handed back to deallocate
    sync input port packetOverrideReturnIn: Fw.BufferSend

    @ Return packetOverrideIn reload buffers to the persistence component that sent them
    output port packetOverrideReturnOut: Fw.BufferSend

    @ Per-packet override reload from the persistence component.
    async input port packetOverrideIn: Fw.BufferSend

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

    @ Set the send state of a group on a section
    async command CONFIGURE_GROUP_SEND(
                                        section: TelemetrySection   @< Section grouping
                                        tlmGroup: FwChanIdType      @< Group Identifier
                                        sendState: TlmSendState     @< Send state (INHERIT_FROM_PARENT follows the section enable)
                                      ) \
      opcode 3

    @ Set the rate logic and Min / Max deltas between successive packets of a group on a section
    async command CONFIGURE_GROUP_RATES(
                                        section: TelemetrySection   @< Section grouping
                                        tlmGroup: FwChanIdType      @< Group Identifier
                                        rateLogic: RateLogic        @< Rate Logic
                                        minDelta: U32               @< Minimum Sched Ticks to send packets on updates when using ON_CHANGE logic
                                        maxDelta: U32               @< Maximum Sched Ticks between packets to send when using EVERY_MAX logic
                                      ) \
      opcode 4

    @ Report a packet's override and effective configuration (PacketConfigReport event)
    async command GET_PACKET_CONFIG(
                                    section: TelemetrySection   @< Section to query
                                    packetId: U32               @< Packet identifier
                                  ) \
      opcode 5

    @ Override the send state of a single packet in a section (rates follow the group unless also overridden)
    async command OVERRIDE_PACKET_SEND(
                                        section: TelemetrySection   @< Section to configure
                                        packetId: U32               @< Packet identifier
                                        sendState: TlmSendState     @< Send state (INHERIT_FROM_PARENT follows the group)
                                      ) \
      opcode 6

    @ Override the rate logic and thresholds of a single packet (send state follows the group unless also overridden)
    async command OVERRIDE_PACKET_RATES(
                                         section: TelemetrySection   @< Section to configure
                                         packetId: U32               @< Packet identifier
                                         rateLogic: RateLogic        @< Rate logic
                                         minDelta: U32               @< Minimum Sched ticks between sends (ON_CHANGE_MIN logic)
                                         maxDelta: U32               @< Maximum Sched ticks between sends (EVERY_MAX logic)
                                       ) \
      opcode 7

    @ Clear a single packet's override. The packet follows its group again.
    async command CLEAR_PACKET_OVERRIDE(
                                         section: TelemetrySection   @< Section to clear
                                         packetId: U32               @< Packet identifier
                                       ) \
      opcode 8

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
      severity activity high \
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

    @ A packetOverrideIn reload record referenced a packet id not in this deployment; the record was skipped
    event ReloadRecordUnknownPacket(
                                     packetId: U32 @< The packet id in the record
                                   ) \
      severity warning low \
      id 7 \
      format "packetOverrideIn reload record for unknown packet id {} skipped"

    @ A packetOverrideIn reload record carried a section out of range; the record was skipped
    event ReloadRecordInvalid(
                               packetId: U32 @< The packet id in the record
                             ) \
      severity warning low \
      id 14 \
      format "packetOverrideIn reload record for packet id {} skipped: section out of range"

    @ Could not allocate a buffer to mirror a per-packet override out packetOverrideOut
    event ConfigMirrorAllocFailed \
      severity warning high \
      id 8 \
      format "Could not allocate buffer to mirror packet config" \
      throttle 10

    @ A packet's override and effective configuration, emitted in response to GET_PACKET_CONFIG
    event PacketConfigReport(
                              section: TelemetrySection          @< The section queried
                              packetId: U32                      @< The packet id
                              override: PacketOverrideConfig     @< The stored override (GROUP_DEFAULT if none)
                              effective: TlmSendConfig           @< The effective configuration after applying the override to the group
                            ) \
      severity activity high \
      id 9 \
      format "Packet config: section {} packet id {} override {} effective {}"

    @ A per-packet override was applied by command; carries the stored override
    event PacketOverrideSet(
                             section: TelemetrySection          @< The section configured
                             packetId: U32                      @< The packet id
                             override: PacketOverrideConfig     @< The override now stored
                           ) \
      severity activity high \
      id 10 \
      format "Packet override set: section {} packet id {} override {}"

    @ A per-packet override was cleared by command; the packet reverts to its group policy
    event PacketOverrideCleared(
                                 section: TelemetrySection @< The section the override was cleared in
                                 packetId: U32 @< The packet id
                               ) \
      severity activity high \
      id 11 \
      format "Packet override cleared: section {} packet id {} reverts to group policy"

    @ A packetOverrideIn reload buffer was malformed: a record failed to deserialize; later records were not applied
    event ConfigReloadFailed(
                              status: Fw.DeserialStatus @< The deserialization status that ended the reload
                            ) \
      severity warning high \
      id 12 \
      format "packetOverrideIn reload failed with status {}"

    @ A command carried an invalid section for a per-packet operation
    event InvalidPacketConfig(
                               packetId: U32 @< The packet id in the rejected config
                             ) \
      severity warning low \
      id 13 \
      format "Packet config for packet id {} rejected: invalid section"

    @ A group's send state was set by CONFIGURE_GROUP_SEND
    event GroupSendStateSet(
                             section: TelemetrySection   @< Section grouping
                             tlmGroup: FwChanIdType      @< Group identifier
                             sendState: TlmSendState     @< New send state
                           ) \
      severity activity high \
      id 15 \
      format "Section {} group {} send state set to {}"

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
