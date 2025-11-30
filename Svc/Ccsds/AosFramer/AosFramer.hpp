// ======================================================================
// \title  AosFramer.hpp
// \author Will MacCormack (Aos Modifications)
// \brief  hpp file for AosFramer component implementation class
// \details modified from thomas-bc's TmFramer
// ======================================================================

#ifndef Svc_Ccsds_AosFramer_HPP
#define Svc_Ccsds_AosFramer_HPP

#include "Svc/Ccsds/AosFramer/AosFramerComponentAc.hpp"
#include "Svc/Ccsds/Types/FppConstantsAc.hpp"
#include "Svc/Ccsds/Types/SpacePacketHeaderSerializableAc.hpp"
#include "Svc/Ccsds/Types/TMHeaderSerializableAc.hpp"
#include "Svc/Ccsds/Types/TMTrailerSerializableAc.hpp"

namespace Svc {

namespace Ccsds {

class AosFramer final : public AosFramerComponentBase {
    friend class AosFramerTester;

    static constexpr U8 IDLE_DATA_PATTERN = 0x44;

    enum class BufferOwnershipState {
        NOT_OWNED,  //!< The buffer is currently not owned by the AosFramer
        OWNED,      //!< The buffer is currently owned by the AosFramer
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
    void configure(U32 fixedFixedSize,           //!< Number of bytes in each AOS SDL Frame
                   bool frameErrorControlField,  //!< Whether to enable the frame error control field
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
  private:
    //! Fill the frame buffer with an Idle Packet to complete the frame data field
    //! as per CCSDS AOS Protocol paragraph 4.2.2.5. Idle packet is inserted at the
    //! start_index index of the frame buffer, and fills it up to the end minus CRC
    void fill_with_idle_packet(Fw::SerializeBufferBase& serializer);

    // ----------------------------------------------------------------------
    // Members
    // ----------------------------------------------------------------------
  private:
    // Because the AOS protocol use fixed width frames, and only one frame is in transit between ComQueue and
    // ComInterface at a time, we can use a member fixed-size buffer to hold the frame data
    U8 m_frameBuffer[ComCfg::AosMaxFrameFixedSize];                    //!< Buffer to hold the frame data
    BufferOwnershipState m_bufferState = BufferOwnershipState::OWNED;  //!< whether m_frameBuffer is owned by AosFramer

    // Current implementation uses a single virtual channel, so we can use a single virtual frame count
    U32 m_virtualFrameCount;  //!< Virtual Frame Count - 24 bits - wraps around at 16,777,216
    FwSizeType m_fixedFrameSize;  //!< AOS Fixed Frame size for this particular AosFramer instance
    FwSizeType m_fecf;            //!< AOS Frame Error Control Field presence
};

}  // namespace Ccsds
}  // namespace Svc

#endif
