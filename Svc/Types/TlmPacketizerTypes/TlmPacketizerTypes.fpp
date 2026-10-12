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

    @ Telemetry send policy (enable + rate), shared by the per-group config and per-packet override.
    struct TlmSendConfig {
      enabled: Fw.Enabled       @< Enable / disable telemetry output
      forceEnabled: Fw.Enabled  @< Force telemetry output even if disabled
      rateLogic: RateLogic      @< Rate logic configuration
      min: U32                  @< Minimum Sched ticks between sends when using ON_CHANGE_MIN logic
      max: U32                  @< Maximum Sched ticks between sends when using EVERY_MAX logic
    }

    @ Source of a packet's effective policy
    enum PacketConfigSource {
      GROUP_DEFAULT,  @< Packet follows its group-derived policy
      OVERRIDDEN,     @< Packet has an active per-packet override
    }
}
