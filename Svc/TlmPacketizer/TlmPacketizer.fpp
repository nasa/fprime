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

    @ Result of stopping data product recording for a group
    enum DpStopStatus : U8 {
      NOT_RECORDING = 0     @< The group was not being recorded
      PARTIAL_NOT_SENT = 1  @< No partially filled container was pending
      PARTIAL_SENT = 2      @< A partially filled container was sent
    }

    @ Reason a data product recording command was rejected
    enum DpRejectReason : U8 {
      START_INVALID_GROUP = 0         @< START_DP_RECORDING group exceeds MAX_CONFIGURABLE_TLMPACKETIZER_GROUP
      START_INVALID_PACKET_COUNT = 1  @< START_DP_RECORDING packetsPerContainer is zero or too large
      START_NO_PACKETS_IN_GROUP = 2   @< START_DP_RECORDING group has no packets in the packet list
      STOP_INVALID_GROUP = 3          @< STOP_DP_RECORDING group exceeds MAX_CONFIGURABLE_TLMPACKETIZER_GROUP
      START_PORTS_NOT_CONNECTED = 4   @< START_DP_RECORDING issued while the product ports are not connected
    }

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

    @ Data product get port
    product get port productGetOut

    @ Data product send port
    product send port productSendOut

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

    @ Start recording the packets of a telemetry group as data products
    async command START_DP_RECORDING(
                                      tlmGroup: FwChanIdType              @< Group Identifier
                                      packetsPerContainer: FwSizeType     @< Number of packets per data product container
                                      $priority: FwDpPriorityType         @< Data product priority
                                    ) \
      opcode 6

    @ Stop recording the packets of a telemetry group as data products, sending any partial container
    async command STOP_DP_RECORDING(
                                     tlmGroup: FwChanIdType @< Group Identifier
                                   ) \
      opcode 7

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

    @ Data product recording of a telemetry group started
    event DpRecordingStarted(
                              tlmGroup: FwChanIdType          @< Group Identifier
                              packetsPerContainer: FwSizeType @< Number of packets per data product container
                              $priority: FwDpPriorityType     @< Data product priority
                            ) \
      severity activity high \
      id 7 \
      format "Started data product recording of group {}: {} packets per container, priority {}"

    @ Data product recording of a telemetry group stopped; the counts cover the recording since START_DP_RECORDING
    event DpRecordingStopped(
                              tlmGroup: FwChanIdType  @< Group Identifier
                              status: DpStopStatus    @< Partial container status
                              packetsRecorded: U32    @< Packets recorded into containers
                              containersSent: U32     @< Containers sent
                              packetsDropped: U32     @< Packets dropped because no container could be obtained
                            ) \
      severity activity high \
      id 8 \
      format "Stopped data product recording of group {}: {}, {} packets recorded, {} containers sent, {} packets dropped"

    @ Data product recording command rejected
    event DpRecordingRejected(
                               tlmGroup: FwChanIdType  @< Group Identifier
                               reason: DpRejectReason  @< Rejection reason
                             ) \
      severity warning low \
      id 9 \
      format "Rejected data product recording command for group {}: {}"

    @ Failed to get a data product container; the packet was dropped
    event DpBufferError(
                         tlmGroup: FwChanIdType @< Group Identifier
                         $size: FwSizeType      @< The requested container data size
                       ) \
      severity warning high \
      id 10 \
      format "Failed to get data product container for group {} of {} bytes" \
      throttle 10
    
    # ----------------------------------------------------------------------
    # Telemetry
    # ----------------------------------------------------------------------

    @ Telemetry send level
    telemetry GroupConfigs: SectionConfigs id 0
    telemetry SectionEnabled: SectionEnabled id 1

    array TelemetrySendSection = [NUM_CONFIGURABLE_TLMPACKETIZER_GROUPS] FwIndexType
    array TelemetrySendPortMap = [TelemetrySection.NUM_SECTIONS] TelemetrySendSection default TELEMETRY_SEND_PORT_MAPPING

    # ----------------------------------------------------------------------
    # Data products
    # ----------------------------------------------------------------------

    @ Telemetry group of the packets in a container; first record of every container
    product record TlmGroupRecord: FwChanIdType id 0

    @ One packetized telemetry packet as sent on PktSend: descriptor, packet ID, time tag, channel values
    product record TlmPacketRecord: U8 array id 1

    @ Container of TlmPacketRecords belonging to a single telemetry group
    product container TlmPacketContainer id 0 default priority 10

  }

}
