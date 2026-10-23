#####
# TlmPacketizer Ports:
#
# Ports and types for Svc::TlmPacketizer
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

    @ A single per-packet config record
    struct PacketConfigEntry {
        packetId: U32                 @< Packet identifier
        section: TelemetrySection     @< Section the policy applies to
        config: TlmSendConfig         @< Enable + rate policy for this packet/section
        source: PacketConfigSource    @< Active override or cleared (group-derived)
    }
}
