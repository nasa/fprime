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
}
