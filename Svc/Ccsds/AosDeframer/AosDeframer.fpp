module Svc {
module Ccsds {
    @ Deframer for the AOS Space Data Link Protocol
    @ Per CCSDS 732.0-B-5 (5th Edition) - AOS Space Data Link Protocol
    @ Supports M_PDU (Multiplexing PDU) data field service with optional:
    @ - Frame Error Control Field (FECF) per Section 4.1.6
    @ - Space Packet Protocol (SPP) extraction per CCSDS 133.0-B-2
    @ - Encapsulation Packet Protocol (EPP) extraction per CCSDS 133.1-B-3
    passive component AosDeframer {

        import Deframer

        @ Port to notify of a deframing error
        output port errorNotify: Ccsds.ErrorNotify

        @ Deframing received an invalid SCID (CCSDS 732.0-B-5 Section 4.1.2.2)
        event InvalidSpacecraftId(transmitted: U16, configured: U16) \
            severity warning low \
            format "Invalid Spacecraft ID Received. Received: {} | Deframer configured with: {}"

        @ Deframing received an invalid frame length
        event InvalidFrameLength(expected: U32, actual: FwSizeType) \
            severity warning high \
            format "Frame length mismatch. Expected: {} | Received: {}"

        @ Deframing received an invalid VCID (CCSDS 732.0-B-5 Section 4.1.2.3)
        event InvalidVcId(transmitted: U8, configured: U8) \
            severity activity low \
            format "Invalid Virtual Channel ID Received. Frame contained: {} | Deframer configured with: {}"

        @ Deframing received an invalid checksum (CCSDS 732.0-B-5 Section 4.1.6)
        event InvalidCrc(transmitted: U16, computed: U16) \
            severity warning high \
            format "Invalid CRC received. Trailer specified: {} | Computed on board: {}"

        @ Deframing received an invalid transfer frame version (CCSDS 732.0-B-5 Section 4.1.2.2.2)
        event InvalidTfvn(transmitted: U8, expected: U8) \
            severity warning high \
            format "Invalid Transfer Frame Version Number. Received: {} | Expected: {}"

        @ Deframing received an invalid EPP packet (CCSDS 133.1-B-3)
        event InvalidEppPacket(version: U8, reason: string size 40) \
            severity warning high \
            format "Invalid EPP Packet. Version: {} | Reason: {}"

        @ Frame was received with idle-only data (CCSDS 732.0-B-5 Section 4.1.4.2.2.4)
        event IdleFrame() \
            severity activity low \
            format "Received frame containing only idle data"

        @ Telemetry for tracking received frames per Virtual Channel
        telemetry FrameCount: U32 \
            format "{} frames received"

        @ Telemetry for tracking extracted packets
        telemetry PacketCount: U32 \
            format "{} packets extracted"

        @ Telemetry for tracking CRC errors
        telemetry CrcErrorCount: U32 \
            format "{} CRC errors"

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
