// ======================================================================
// \title  SpacePacketIdleFiller.hpp
// \author claradavisb
// \brief  hpp file for SpacePacketIdleFiller component implementation class
// ======================================================================

#ifndef Svc_Ccsds_SpacePacketIdleFiller_HPP
#define Svc_Ccsds_SpacePacketIdleFiller_HPP

#include "Svc/Ccsds/SpacePacketIdleFiller/SpacePacketIdleFillerComponentAc.hpp"
#include "Svc/Ccsds/Types/FppConstantsAc.hpp"
#include "Svc/Ccsds/Types/SpacePacketHeaderSerializableAc.hpp"
#include "Svc/Ccsds/Types/TMHeaderSerializableAc.hpp"
#include "Svc/Ccsds/Types/TMTrailerSerializableAc.hpp"

namespace Svc {

namespace Ccsds {

class SpacePacketIdleFiller final : public SpacePacketIdleFillerComponentBase {
    friend class SpacePacketIdleFillerTester;

    static_assert(ComCfg::TmFrameFixedSize > TMHeader::SERIALIZED_SIZE + TMTrailer::SERIALIZED_SIZE,
                  "TM Frame Fixed Size must be large enough to hold a header, a trailer, and data");

    //! Largest buffer this component can emit: a whole transfer frame data field
    static constexpr FwSizeType MAX_FILL_SIZE =
        ComCfg::TmFrameFixedSize - (TMHeader::SERIALIZED_SIZE + TMTrailer::SERIALIZED_SIZE);

    //! Smallest well-formed space packet: a header plus one byte of data
    static constexpr FwSizeType MIN_IDLE_PACKET_SIZE = SpacePacketHeader::SERIALIZED_SIZE + 1;

    //! Fill byte, matching the pattern Svc.Ccsds.TmFramer emits
    static constexpr U8 IDLE_DATA_PATTERN = 0x44;

    enum class BufferOwnershipState {
        NOT_OWNED,  //!< m_fillBuffer is downstream
        OWNED,      //!< m_fillBuffer is available for the next buffer
    };

  public:
    explicit SpacePacketIdleFiller(const char* const compName);
    ~SpacePacketIdleFiller();

    SpacePacketIdleFiller(const SpacePacketIdleFiller&) = delete;
    SpacePacketIdleFiller& operator=(const SpacePacketIdleFiller&) = delete;
    SpacePacketIdleFiller(SpacePacketIdleFiller&&) = delete;
    SpacePacketIdleFiller& operator=(SpacePacketIdleFiller&&) = delete;

    //! Set the size every emitted buffer is padded to
    void configure(FwSizeType targetSize);

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Pad the incoming buffer to the configured target
    void dataIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) override;

    //! Reclaim ownership of the emitted buffer
    void dataReturnIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) override;

    //! Pass the downstream status through unchanged
    void comStatusIn_handler(FwIndexType portNum, Fw::Success& condition) override;

    // ----------------------------------------------------------------------
    // Helpers
    // ----------------------------------------------------------------------

    //! Return a buffer that will not be emitted to its sender, and release the com status
    void dropBuffer(Fw::Buffer& data, const ComCfg::FrameContext& context);

    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! Buffer holding the padded copy;
    U8 m_fillBuffer[MAX_FILL_SIZE];

    //! Size every emitted buffer is padded to
    FwSizeType m_targetSize;

    //! Whether configure() has been called
    bool m_configured;

    //! Whether m_fillBuffer is available
    BufferOwnershipState m_bufferState;
};

}  // namespace Ccsds

}  // namespace Svc

#endif
