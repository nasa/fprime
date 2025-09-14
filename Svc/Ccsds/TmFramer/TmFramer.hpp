// ======================================================================
// \title  TmFramer.hpp
// \author thomas-bc
// \brief  hpp file for TmFramer component implementation class
// ======================================================================

#ifndef Svc_Ccsds_TmFramer_HPP
#define Svc_Ccsds_TmFramer_HPP

#include "Svc/Ccsds/TmFramer/TmFramerComponentAc.hpp"
#include "Svc/Ccsds/Types/FppConstantsAc.hpp"
#include "Svc/Ccsds/Types/SpacePacketHeaderSerializableAc.hpp"
#include "Svc/Ccsds/Types/TMHeaderSerializableAc.hpp"
#include "Svc/Ccsds/Types/TMTrailerSerializableAc.hpp"

namespace Svc {

namespace Ccsds {

class TmFramer final : public TmFramerComponentBase {
    friend class TmFramerTester;

    static_assert(ComCfg::TmFrameFixedSize > TMHeader::SERIALIZED_SIZE + TMTrailer::SERIALIZED_SIZE,
                  "TM Frame Fixed Size must be at least large enough to hold header, trailer and data");
    // These are to ensure the frame can hold the packet buffer, its SP header and an idle packet of 1 byte
    // This is because TM specifies a frame to be padded with an idle packet of at least 1 byte of idle data
    static_assert(
        ComCfg::TmFrameFixedSize >= FW_COM_BUFFER_MAX_SIZE + (2 * SpacePacketHeader::SERIALIZED_SIZE) + 1,
        "TM Frame Fixed Size must be at least large enough to hold a full com buffer, 2 SP headers and 1 byte");
    static_assert(
        ComCfg::TmFrameFixedSize >= FW_FILE_BUFFER_MAX_SIZE + (2 * SpacePacketHeader::SERIALIZED_SIZE) + 1,
        "TM Frame Fixed Size must be at least large enough to hold a full com buffer, 2 SP headers and 1 byte");

    static constexpr U8 IDLE_DATA_PATTERN = 0x44;

    enum class BufferOwnershipState {
        NOT_OWNED,  //!< The buffer is currently not owned by the TmFramer
        OWNED,      //!< The buffer is currently owned by the TmFramer
    };

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct TmFramer object
    TmFramer(const char* const compName  //!< The component name
    );

    //! Destroy TmFramer object
    ~TmFramer();

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
    //! This is essentially the CCSDS TM VCP.request Service Primitive, with
    //! Packet=data and GVCID implicitly passed in context (TM Protocol 3.3.3.2)
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
  private:
    //! Fill the frame buffer with an Idle Packet to complete the frame data field
    //! as per CCSDS TM Protocol paragraph 4.2.2.5. Idle packet is inserted at the
    //! start_index index of the frame buffer, and fills it up to the end minus CRC
    void fill_with_idle_packet(Fw::SerializeBufferBase& serializer);

    // ----------------------------------------------------------------------
    // Members
    // ----------------------------------------------------------------------
  private:
    // Because the TM protocol use fixed width frames, and only one frame is in transit between ComQueue and
    // ComInterface at a time, we can use a member fixed-size buffer to hold the frame data
    U8 m_frameBuffer[ComCfg::TmFrameFixedSize];                        //!< Buffer to hold the frame data
    BufferOwnershipState m_bufferState = BufferOwnershipState::OWNED;  //!< whether m_frameBuffer is owned by TmFramer

    // Current implementation uses a single virtual channel, so we can use a single virtual frame count
    U8 m_masterFrameCount;   //!< Master Frame Count - 8 bits - wraps around at 255
    U8 m_virtualFrameCount;  //!< Virtual Frame Count - 8 bits - wraps around at 255
};

}  // namespace Ccsds
}  // namespace Svc

#endif
