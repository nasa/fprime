// ======================================================================
// \title  TmFramer.hpp
// \author thomas-bc
// \brief  hpp file for TmFramer component implementation class
// ======================================================================

#ifndef Svc_Ccsds_TmFramer_HPP
#define Svc_Ccsds_TmFramer_HPP

#include "Svc/Ccsds/TmFramer/TmFramerComponentAc.hpp"
#include "Svc/Ccsds/Types/FppConstantsAc.hpp"
#include "Svc/Ccsds/Types/TMHeaderSerializableAc.hpp"
#include "Svc/Ccsds/Types/TMTrailerSerializableAc.hpp"

namespace Svc {

namespace Ccsds {

class TmFramer final : public TmFramerComponentBase {
    friend class TmFramerTester;

    static_assert(ComCfg::TmFrameFixedSize > TMHeader::SERIALIZED_SIZE + TMTrailer::SERIALIZED_SIZE,
                  "TM Frame Fixed Size must be at least large enough to hold header, trailer and data");

    //! Size of the frame data field: every dataIn payload must be exactly this size (Svc.Ccsds.TmDataFieldSize)
    static constexpr FwSizeType TmPayloadCapacity =
        ComCfg::TmFrameFixedSize - (TMHeader::SERIALIZED_SIZE + TMTrailer::SERIALIZED_SIZE);
    static_assert(static_cast<FwSizeType>(TMHeader::SERIALIZED_SIZE) == static_cast<FwSizeType>(TmHeaderSize),
                  "Svc.Ccsds.TmHeaderSize must match TMHeader");
    static_assert(static_cast<FwSizeType>(TMTrailer::SERIALIZED_SIZE) == static_cast<FwSizeType>(TmTrailerSize),
                  "Svc.Ccsds.TmTrailerSize must match TMTrailer");
    static_assert(TmPayloadCapacity == static_cast<FwSizeType>(TmDataFieldSize),
                  "Svc.Ccsds.TmDataFieldSize must match the TM data field");

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
