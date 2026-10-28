#####
# TlmPacketizer types:
#
# Types for the TlmPacketizer
#####

module Svc {

    @ Send policy of a group or packet. Resolution: packet -> group -> section enable.
    enum TlmSendState : U8 {
      INHERIT_FROM_PARENT,          @< Packet: use the group's state; group: use the section enable (default)
      SEND_WHEN_SECTION_ENABLED,    @< Send only while the section is enabled
      SEND_ALWAYS,                  @< Send; the section enable is ignored
      SEND_NEVER,                   @< Never send
    }

    @ Enumeration for rate logic types for telemetry groups
    enum RateLogic : U8 {
      EVERY_MAX,                    @< Send every MAX ticks between sends.
      ON_CHANGE_MIN,                @< Send on updates after MIN ticks since last send.
      ON_CHANGE_MIN_OR_EVERY_MAX,   @< Send on updates after MIN ticks since last send OR at MAX ticks between sends.
    }
}
