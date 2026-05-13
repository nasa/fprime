module Svc {
module Ccsds {
    @ Deframer for the AOS Space Data Link Protocol
    @ Per CCSDS 732.0-B-5 (5th Edition) - AOS Space Data Link Protocol
    @ Supports M_PDU (Multiplexing PDU) data field service with optional:
    @ - Frame Error Control Field (FECF) per Section 4.1.6
    @ - Space Packet Protocol (SPP) extraction per CCSDS 133.0-B-2
    @ - Encapsulation Packet Protocol (EPP) extraction per CCSDS 133.1-B-3
    passive component AosDeframer {

        constant NumVcs = 1

        @ Default upper bound on the size of a single packet (Space Packet or
        @ Encapsulation Packet) accepted by the deframer. The packet-length
        @ field of a CCSDS Space Packet is U16 (max 65535 + 1 byte = 65536),
        @ and Encapsulation Packets can in principle be larger. Capping at
        @ 65536 by default keeps a malformed or hostile packet-length read
        @ from the wire from being passed directly to the buffer allocator.
        @ Override per-instance via configure().
        constant DefaultMaxPacketSize = 65536

        import Deframer

        @ Port to notify of a deframing error
        output port errorNotify: Ccsds.ErrorNotify

        @ Buffer allocation and deallocation for packets that span across multiple AOS frames
        import Svc.BufferAllocation

        include "AosDeframerEvents.fppi"
        include "AosDeframerTelem.fppi"

        ###############################################################################
        # Standard AC Ports: Required for Channels, Events, Commands, and Parameters  #
        ###############################################################################
        @ Port for requesting the current time
        time get port timeCaller

        @ Port for sending textual representation of events
        text event port logTextOut

        @ Port for sending events to downlink
        event port logOut

        @ Port for sending telemetry channels to downlink
        telemetry port tlmOut

        @ Port to return the value of a parameter
        param get port prmGetOut

        @Port to set the value of a parameter
        param set port prmSetOut

    }
}
}
