#####
# TlmPacketizer Ports:
#
# A port enabling sections of the TlmPacketizer
#####

module Svc {

    @ Enumeration for rate logic types for telemetry groups
    enum RateLogic {
      SILENCED,                     @< No logic applied. Does not send group and freezes counter.
      EVERY_MAX,                    @< Send every MAX ticks between sends.
      ON_CHANGE_MIN,                @< Send on updates after MIN ticks since last send.
      ON_CHANGE_MIN_OR_EVERY_MAX,   @< Send on updates after MIN ticks since last send OR at MAX ticks between sends.
    }

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
