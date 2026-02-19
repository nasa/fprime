// ======================================================================
// \title  AosDeframer.hpp
// \author Auto-generated
// \brief  hpp file for AosDeframer component implementation class
//
// Deframer for the AOS Space Data Link Protocol per CCSDS 732.0-B-5.
// Supports M_PDU data field service with:
// - Frame Error Control Field (FECF) validation
// - Space Packet Protocol (SPP) extraction
// - Encapsulation Packet Protocol (EPP) extraction per CCSDS 133.1-B-3
// ======================================================================

#ifndef Svc_Ccsds_AosDeframer_HPP
#define Svc_Ccsds_AosDeframer_HPP

#include "Svc/Ccsds/AosDeframer/AosDeframerComponentAc.hpp"
#include "Svc/Ccsds/Types/AOSHeaderSerializableAc.hpp"
#include "Svc/Ccsds/Types/AOSTrailerSerializableAc.hpp"
#include "Svc/Ccsds/Types/FppConstantsAc.hpp"
#include "Svc/Ccsds/Types/M_PDUHeaderSerializableAc.hpp"
#include "Svc/Ccsds/Types/TfvnEnumAc.hpp"

namespace Svc {
namespace Ccsds {

class AosDeframer : public AosDeframerComponentBase {
    friend class AosDeframerTester;

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct AosDeframer object
    AosDeframer(const char* const compName  //!< The component name
    );

    //! Destroy AosDeframer object
    ~AosDeframer();

    //! \brief Configure the AosDeframer with mission-specific parameters
    //!
    //! Must be called before any frames are processed. Configures the deframer
    //! for the expected AOS frame format per CCSDS 732.0-B-5.
    //!
    //! \param fixedFrameSize Fixed size of AOS frames in bytes (per Section 4.1.1)
    //! \param frameErrorControlField Whether FECF is present (per Section 4.1.6)
    //! \param spacecraftId The spacecraft ID to accept (10 bits, per Section 4.1.2.2)
    //! \param vcId The virtual channel ID to accept (6 bits, per Section 4.1.2.3)
    //! \param acceptAllVcid If true, accept frames from all virtual channels
    //! \param pvnMask Bitmask of Packet Version Numbers to extract (SPP=0x1, EPP=0x8)
    //!
    void configure(U32 fixedFrameSize,
                   bool frameErrorControlField,
                   U16 spacecraftId = ComCfg::SpacecraftId,
                   U8 vcId = 0,
                   bool acceptAllVcid = true,
                   U8 pvnMask = PvnBitfield::SPP_MASK | PvnBitfield::EPP_MASK);

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for dataIn
    //!
    //! Port to receive framed AOS data. This is essentially the CCSDS AOS
    //! VC_RECEIVE.indication Service Primitive (per Section 3.4.3.2)
    void dataIn_handler(FwIndexType portNum,  //!< The port number
                        Fw::Buffer& data,
                        const ComCfg::FrameContext& context) override;

    //! Handler implementation for dataReturnIn
    //!
    //! Port receiving back ownership of sent packet buffers
    void dataReturnIn_handler(FwIndexType portNum,  //!< The port number
                              Fw::Buffer& data,     //!< The buffer
                              const ComCfg::FrameContext& context) override;

    // ----------------------------------------------------------------------
    // Private helper methods
    // ----------------------------------------------------------------------

    //! Helper method to send an error notification if the errorNotify port is connected
    //! \param error The error to send
    void errorNotifyHelper(Svc::Ccsds::FrameError error);

    //! Parse the AOS Primary Header per CCSDS 732.0-B-5 Section 4.1.2
    //! \param data The frame buffer
    //! \param context The frame context to update
    //! \return true if header is valid, false otherwise
    bool parseAndValidateHeader(Fw::Buffer& data, ComCfg::FrameContext& context);

    //! Validate the Frame Error Control Field (CRC) per CCSDS 732.0-B-5 Section 4.1.6
    //! \param data The frame buffer
    //! \param vc The virtual channel state (for CRC error counter)
    //! \return true if CRC is valid, false otherwise
    bool validateFecf(Fw::Buffer& data, AosDeframerVc& vc);

    //! Parse the M_PDU header and extract packets per CCSDS 732.0-B-5 Section 4.1.4.2
    //! \param vc The virtual channel state
    //! \param data The frame buffer (positioned after AOS primary header)
    //! \param context The frame context
    void extractPackets(AosDeframerVc& vc, Fw::Buffer& data, ComCfg::FrameContext& context);

    //! Extract a Space Packet from the M_PDU data zone per CCSDS 133.0-B-2
    //! \param vc The virtual channel state
    //! \param payloadStart Pointer to start of packet data within the incoming frame buffer
    //! \param payloadSize Available bytes in the data zone
    //! \param context The frame context
    //! \return Number of bytes consumed (packet size), or 0 if incomplete
    //! \note payloadStart points into the incoming frame buffer. In synchronous port mode this
    //!       is safe since the frame is not returned until extractPackets returns. For async
    //!       consumers a copy may be needed before the frame buffer is returned.
    FwSizeType extractSppPacket(AosDeframerVc& vc, U8* payloadStart, FwSizeType payloadSize, ComCfg::FrameContext& context);

    //! Extract an Encapsulation Packet from the M_PDU data zone per CCSDS 133.1-B-3
    //! \param vc The virtual channel state
    //! \param payloadStart Pointer to start of packet data within the incoming frame buffer
    //! \param payloadSize Available bytes in the data zone
    //! \param context The frame context
    //! \return Number of bytes consumed (packet size), or 0 if incomplete/invalid
    //! \note See extractSppPacket note regarding buffer ownership.
    FwSizeType extractEppPacket(AosDeframerVc& vc, U8* payloadStart, FwSizeType payloadSize, ComCfg::FrameContext& context);

    //! Determine packet type from first byte (PVN field)
    //! \param firstByte First byte of packet
    //! \return Packet Version Number (0 for SPP, 7 for EPP)
    static U8 getPacketVersion(U8 firstByte);

    //! Check if a packet type is enabled in the PVN mask
    //! \param pvn Packet Version Number
    //! \return true if packet type should be extracted
    bool isPacketTypeEnabled(U8 pvn) const;

    //! Per-virtual-channel state, mirroring the AosFramer::AosVc pattern for future multi-VC support
    struct AosDeframerVc {
        U8 vcStructIndex = 0xFF;     //!< Index into VC array for this vc struct
        U8 virtualChannelId = 0;     //!< VCID for this virtual channel
        U8 pvnMask = PvnBitfield::SPP_MASK | PvnBitfield::EPP_MASK;  //!< Bitmask of enabled PVNs

        // Telemetry counters
        U32 frameCount = 0;          //!< Total frames received on this VC
        U32 packetCount = 0;         //!< Total packets extracted from this VC
        U32 crcErrorCount = 0;       //!< Total CRC errors for this VC
        U32 vcFrameCount = 0;        //!< Last received virtual channel frame count

        // Spanning packet state (for packets that span multiple frames)
        // Per CCSDS 732.0-B-5 Section 4.1.4.2.2.3
        struct SpanningPacketState {
            U8 buffer[ComCfg::AosMaxFrameFixedSize];  //!< Buffer for partial packet data
            FwSizeType bytesReceived = 0;              //!< Bytes received so far
            FwSizeType expectedSize = 0;               //!< Expected total packet size (0 if unknown)
            U8 pvn = 0;                                //!< Packet Version Number of spanning packet
            bool active = false;                       //!< Whether a spanning packet is in progress
        } spanningPacket;
    };

  private:
    // ----------------------------------------------------------------------
    // Private helper methods (continued)
    // ----------------------------------------------------------------------

    //! Append data to the active spanning packet buffer, completing it if possible
    //! \param vc The virtual channel state
    //! \param data Pointer to data bytes to append
    //! \param size Number of bytes to append
    //! \param context Frame context for output
    //! \return true if the spanning packet is now complete and was sent downstream
    bool appendToSpanningPacket(AosDeframerVc& vc, U8* data, FwSizeType size, ComCfg::FrameContext& context);

    //! Map frame context onto the appropriate virtual channel struct
    //! TODO: Implement multi-VC support; currently always returns m_vcs[0]
    AosDeframerVc& getVcStruct(const ComCfg::FrameContext& context);

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    // Frame-level configuration parameters (set via configure())
    U32 m_fixedFrameSize = 0;            //!< Fixed frame size in bytes
    bool m_fecfEnabled = true;           //!< Whether FECF is enabled
    U16 m_spacecraftId = 0;              //!< Expected spacecraft ID (10 bits)
    bool m_acceptAllVcid = true;         //!< Accept frames from all VCIDs

    //! TODO: Implement multiple VCs - currently always returns m_vcs[0]
    AosDeframerVc m_vcs[1];  //!< Our one AOS Virtual Channel (for now)
};

}  // namespace Ccsds
}  // namespace Svc

#endif
