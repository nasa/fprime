// ======================================================================
// \title  AosFramer.hpp
// \author Will MacCormack (Aos Modifications)
// \brief  hpp file for AosFramer component implementation class
// \details modified from thomas-bc's TmFramer
// ======================================================================

#ifndef Svc_Ccsds_AosFramer_HPP
#define Svc_Ccsds_AosFramer_HPP

#include "Svc/Ccsds/AosFramer/AosFramerComponentAc.hpp"
#include "Svc/Ccsds/Types/AOSHeaderSerializableAc.hpp"
#include "Svc/Ccsds/Types/AOSTrailerSerializableAc.hpp"
#include "Svc/Ccsds/Types/FppConstantsAc.hpp"
#include "Svc/Ccsds/Types/M_PDUHeaderSerializableAc.hpp"
#include "Svc/Ccsds/Types/SpacePacketHeaderSerializableAc.hpp"
#include "Svc/Ccsds/Types/TfvnEnumAc.hpp"

namespace Svc {

namespace Ccsds {

class AosFramer final : public AosFramerComponentBase {
    friend class AosFramerTester;

    static constexpr U8 SPP_IDLE_DATA_PATTERN = 0x44;
    static constexpr U8 MIN_SPP_LENGTH = 7;

    // Offset to start of payload
    static constexpr U16 START_OF_PAYLOAD = AOSHeader::SERIALIZED_SIZE + M_PDUHeader::SERIALIZED_SIZE;

    enum class BufferOwnershipState {
        NOT_OWNED,  //!< The buffer is currently not owned by the AosFramer
        OWNED,      //!< The buffer is currently owned by the AosFramer
    };

    struct PDU {
        Fw::Buffer packet;             //!< User Packet to be spread across M_PDUs
        ComCfg::FrameContext context;  //!< Context for above user packet
        FwSizeType offset = 0;         //!< Offset into the above packet to write
    };

    struct AosVc {
        U8 vc_struct_index = 0xFF;  //!< Index into VC Array for this vc struct
        U8 virtualChannelId = 1;    //!< VCID for this particular virtual channel
        // Current implementation uses a single virtual channel, so we can use a single virtual frame count
        U32 virtualFrameCount = 0;  //!< Virtual Frame Count - 24 bits - wraps around at 16,777,216

        // Because the AOS protocol use fixed width frames, and only one frame is in transit between ComQueue and
        // ComInterface at a time, we can use a member fixed-size buffer to hold the frame data
        struct FrameBuffer {
            U8 backer[ComCfg::AosMaxFrameFixedSize];                   //!< Buffer to hold the frame data
            Fw::Buffer buffer;                                         //!< Buffer object pointing at frameBufferBacker
            BufferOwnershipState state = BufferOwnershipState::OWNED;  //!< whether m_frameBuffer is owned by AosFramer
        } frame;

        // multi frame per PDU support
        PDU outstanding;

        // Bitfield of supported PVNs for inserted Idle Packets
        // Default to only supporting SPP Idle packets
        U8 idle_packet_types = PvnBitfield::SPP_MASK;

        // SPP Idle packet backstop
        // Technically we'd only use 6 of the 7 bytes at worst
        // cuz the first one had to go into the prev frame
        struct SppIdle {
            U8 backer[MIN_SPP_LENGTH];
            U8 offset = 0;
        } spp_idle;

        // Multi PDU per frame and
        U16 current_payload_offset = 0;        //!< How far into the current PDU we are
        bool past_first_fresh_packet = false;  //!< Past the first fresh packet in an M_PDU
    };

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct AosFramer object
    AosFramer(const char* const compName  //!< The component name
    );

    //! Destroy AosFramer object
    ~AosFramer();

    //! Configure Managed Parameters for this AOS Framer
    //!
    void configure(U32 fixedFixedSize,                  //!< Number of bytes in each AOS SDL Frame
                   bool frameErrorControlField,         //!< Whether to enable the frame error control field
                   U8 idlePvns = PvnBitfield::SPP_MASK  //!< Bitfield of which Packet Version Numbers to use
                                                        //!< for idle packets
                                                        //!< Default to SPP
    );

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for comStatusIn
    //!
    //! Port receiving the general status from the downstream component
    //! indicating it is ready or not-ready for more input
    void comStatusIn_handler(FwIndexType portNum,    //!< The port number
                             Fw::Success& condition  //!< Condition success/failure
                             ) override;

    //! Handler implementation for dataIn
    //!
    //! Port to receive data to frame, in a Fw::Buffer with optional context.
    //! This provides the CCSDS AOS VCP.request Service, with
    //! Packet=data and GVCID & PVN implicitly passed in context (AOS SDL Protocol 3.3.3.2)
    //!
    void dataIn_handler(FwIndexType portNum,  //!< The port number
                        Fw::Buffer& data,
                        const ComCfg::FrameContext& context) override;

    //! Handler implementation for dataReturnIn
    //!
    //! Buffer coming from a deallocate call in a ComDriver component
    void dataReturnIn_handler(FwIndexType portNum,  //!< The port number
                              Fw::Buffer& data,
                              const ComCfg::FrameContext& context) override;

    // ----------------------------------------------------------------------
    // Helpers
    // ----------------------------------------------------------------------

    FwSizeType get_min_size();

  private:
    //! Fill the frame buffer with an Idle Packet to complete the frame data field
    //! as per CCSDS AOS Protocol paragraph 4.2.2.5. Idle packet is inserted at current_payload_offset and fills to end
    //! (minus optional CRC)
    void fill_with_idle_packet(AosVc& vc, const ComCfg::FrameContext& context);

    void serialize_idle_spp_packet(Fw::SerializeBufferBase& serializer, U16 length);

    //! Fill out the Transfer Frame Primary Header (4.1.2)
    void setup_header(const ComCfg::FrameContext& context);

    //! Write the pointer to the first packet header (4.1.4.2.2)
    void setup_m_pdu_header(const ComCfg::FrameContext& context, bool noFresh = false);

    //! Write a buffer into vc & prep to wrap onto next if needed
    void pack_packet(Fw::Buffer& data, const ComCfg::FrameContext& context, FwSizeType offset = 0);

    //! Send the vc frame if we have filled it
    void check_and_send_vc(AosFramer::AosVc& currentVc);

    //! Pack, Idle pad (if requested), and Send (if full) this packet
    void pack_pad_send(Fw::Buffer& data, const ComCfg::FrameContext& context, FwSizeType offset = 0);

    //! Computing Trailing Frame Error Control Field (4.1.6)
    void compute_and_inject_fecf(AosVc& currentVc);

    //! Determine if the Fw::Buffer is within the backing character buffer
    static bool buffer_belongs(Fw::Buffer& buffer, U8 const* start, FwSizeType size);

    //! TODO: Implement multiple VCs
    //! Map frame context onto index into array of Virtual Channel structs
    //! currently returns 0th regardless
    AosVc& get_vc_struct(const ComCfg::FrameContext& context);

    // ----------------------------------------------------------------------
    // Members
    // ----------------------------------------------------------------------
  private:
    // Config Parameters
    bool m_fecf = true;  //!< AOS Frame Error Control Field presence

    AosVc m_vcs[1];  //! Our one AOS Virtual Channel (for now)
};

}  // namespace Ccsds
}  // namespace Svc

#endif
