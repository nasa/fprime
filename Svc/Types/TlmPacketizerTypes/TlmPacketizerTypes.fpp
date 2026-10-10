#####
# TlmPacketizer types:
#
# Types for the TlmPacketizer
#####

module Svc {

    @ Enumeration for rate logic types for telemetry groups
    enum RateLogic {
      SILENCED,                     @< No logic applied. Does not send group and freezes counter.
      EVERY_MAX,                    @< Send every MAX ticks between sends.
      ON_CHANGE_MIN,                @< Send on updates after MIN ticks since last send.
      ON_CHANGE_MIN_OR_EVERY_MAX,   @< Send on updates after MIN ticks since last send OR at MAX ticks between sends.
    }

    @ Per-packet telemetry policy: enable + rate configuration for a single packet.
    @ Mirrors the fields of the legacy per-group configuration, but applied to one packet
    @ so that each packet can be enabled/disabled and rate-controlled independently.
    struct PacketConfig {
      enabled: Fw.Enabled       @< Enable / disable telemetry output for this packet
      forceEnabled: Fw.Enabled  @< Force telemetry output for this packet even if disabled
      rateLogic: RateLogic      @< Rate logic configuration
      min: U32                  @< Minimum Sched ticks between sends when using ON_CHANGE_MIN logic
      max: U32                  @< Maximum Sched ticks between sends when using EVERY_MAX logic
    }
}
