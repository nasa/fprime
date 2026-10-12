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

    struct PacketConfigEntry {
        packetId: U32                 @< Packet identifier
        section: TelemetrySection     @< Section the policy applies to
        config: TlmSendConfig         @< Enable + rate policy for this packet/section
        source: PacketConfigSource    @< Active override or cleared (group-derived)
    }

    @ A fixed-capacity batch of per-packet config entries. Only the first `count` are valid.
    array PacketConfigBatch = [MAX_TLM_PACKET_CONFIG_BATCH] PacketConfigEntry

    @ Pushes a batch of per-packet config between TlmPacketizer and an external persistence component.
    port TlmPacketConfigUpdate(
        count: FwSizeType            @< Number of valid entries in `batch`, in [0, MAX_TLM_PACKET_CONFIG_BATCH]
        batch: PacketConfigBatch     @< Batch of config entries; entries [0, count) are valid
    )
}
