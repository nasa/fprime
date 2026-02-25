        @ Deframing received an invalid SCID (CCSDS 732.0-B-5 Section 4.1.2.2)
        event InvalidSpacecraftId(transmitted: U16, configured: U16) \
            severity warning low \
            format "Invalid Spacecraft ID Received. Received: {} | Deframer configured with: {}"

        @ Deframing received an invalid frame length
        event InvalidFrameLength(actual: FwSizeType, expected: U32) \
            severity warning high \
            format "Frame length mismatch. Received: {} | Expected: {} "

        @ Deframing received a VCID not in the accepted set (CCSDS 732.0-B-5 Section 4.1.2.3)
        event InvalidVcId(transmitted: U8, configured: VCxU8) \
            severity activity low \
            format "Invalid Virtual Channel ID. Frame contained: {} | Accepted VCIDs: {}"

        @ Deframing received an invalid checksum (CCSDS 732.0-B-5 Section 4.1.6)
        event InvalidFecf(transmitted: U16, computed: U16) \
            severity warning high \
            format "Invalid FECF (CRC) received. Trailer specified: {} | Computed on board: {}"

        @ Deframing received an invalid CCSDS Transfer Frame Version Number (CCSDS 732.0-B-5 Section 4.1.2.2.2)
        event InvalidTfvn(transmitted: U8, expected: U8) \
            severity warning high \
            format "Invalid CCSDS Transfer Frame Version Number. Received: {} | Expected: {}"

        @ Deframing encountered an invalid/unsupported packet version number in the M_PDU data zone
        event InvalidPvn(vcId: U8, pvn: U8) \
            severity warning high \
            format "Invalid packet version number {} encountered on VC {}"

        @ Deframing encountered an EPP packet with a malformed header (CCSDS 133.1-B-3)
        event InvalidEppPacket() \
            severity warning high \
            format "Invalid EPP Packet: malformed header"

        @ Deframing encountered an SPP packet with a malformed header (CCSDS 133.0-B-2)
        event InvalidSppPacket() \
            severity warning high \
            format "Invalid SPP Packet: malformed header"

        @ Frame was received with idle-only data on a virtual channel (CCSDS 732.0-B-5 Section 4.1.4.2.2.4)
        event IdleFrame(vcId: U8) \
            severity activity low \
            format "Received frame containing only idle data on VC {}"

        @ Spanning packet buffer allocation failed; packet dropped
        event SpanningPacketAllocFailed(vcId: U8, pvn: U8, packetSize: FwSizeType) \
            severity warning high \
            format "Spanning packet allocation failed on VC {} for {} packet of {} bytes; packet dropped"

        @ Deframing detected a gap/discontinuity in the AOS VC frame count sequence
        event VcFrameCountGap(vcId: U8, received: U32, expected: U32) \
            severity warning high \
            format "VC frame count gap on VC {}. Received {} | Expected next count {}"
