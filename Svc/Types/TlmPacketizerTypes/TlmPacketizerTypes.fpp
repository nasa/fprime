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
}
