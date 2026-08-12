#####
# TlmPacketizer Ports:
#
# A port enabling sections of the TlmPacketizer
#####

module Svc {
    @ Port for enabling/disabling sections of the TlmPacketizer
    port EnableSection(
        section: TelemetrySection @< Section to enable (Primary, Secondary, etc...)
        enabled: Fw.Enabled       @< Enable / Disable Section
    )

    @ Port for configuring section/group rate logic
    port ConfigureGroupRate(
        section: TelemetrySection   @< Section grouping
        tlmGroup: FwChanIdType      @< Group Identifier
        rateLogic: RateLogic        @< Rate Logic
        minDelta: U32               @< Minimum Sched Ticks to send packets on updates when using ON_CHANGE logic
        maxDelta: U32               @< Maximum Sched Ticks between packets to send when using EVERY_MAX logic
    )

    @ Maximum number of per-packet config entries carried in a single push batch.
    @ Sized so one fully-serialized batch stays well within FW_COM_BUFFER_MAX_SIZE.
    constant TLM_PACKET_CONFIG_BATCH_MAX = 32

    @ A single per-packet configuration record: which packet, which section, and its policy.
    @ packetId matches the packet id used by the TlmPacketizer SEND_PKT command.
    struct PacketConfigEntry {
        packetId: U32                @< Packet identifier
        section: TelemetrySection    @< Section the policy applies to
        config: PacketConfig         @< Enable + rate policy for this packet/section
    }

    @ A fixed-capacity batch of per-packet config entries. Only the first `count` are valid.
    array PacketConfigBatch = [TLM_PACKET_CONFIG_BATCH_MAX] PacketConfigEntry

    @ Port pushing a batch of per-packet configuration from the config owner (TlmPacketConfig)
    @ to the packetizer (TlmPacketizer). Batching bounds the number of async messages required
    @ to synchronize many packets (e.g. the full push at boot).
    port TlmPacketConfigUpdate(
        count: FwSizeType            @< Number of valid entries in `batch`, in [0, TLM_PACKET_CONFIG_BATCH_MAX]
        batch: PacketConfigBatch     @< Batch of config entries; entries [0, count) are valid
    )
}
