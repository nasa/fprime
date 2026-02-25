#####
# TlmPacketizer Ports:
#
# A port enabling sections of the TlmPacketizer
#####

module Svc{
    port EnableSection (
        section: TelemetrySection @< Section to enable (Primary, Secondary, etc...)
        enabled: Fw.Enabled       @< Enable / Disable Section
    )
}
