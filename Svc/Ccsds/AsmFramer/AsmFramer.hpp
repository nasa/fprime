// ======================================================================
// \title  AsmFramer.hpp
// \author devin
// \brief  hpp file for AsmFramer component implementation class
// ======================================================================

#ifndef Svc_Ccsds_AsmFramer_HPP
#define Svc_Ccsds_AsmFramer_HPP

#include "Fw/Types/ConstByteArray.hpp"
#include "Svc/Ccsds/AsmFramer/AsmFramerComponentAc.hpp"
#include "config/FppConstantsAc.hpp"

namespace Svc {

namespace Ccsds {

class AsmFramer final : public AsmFramerComponentBase {
    friend class AsmFramerTester;

  public:
    //! Maximum supported ASM size in bytes (Svc.AsmFramerCfg.AsmMaxSize, default 16 = 128 bits, the
    //! rate-1/4 Turbo pattern of CCSDS 131.0-B-5 Section 9.3.2). This is an implementation
    //! bound, not a standard one: the 192-bit rate-1/6 Turbo ASM is not supported unless a
    //! project raises Svc.AsmFramerCfg.AsmMaxSize, which also grows m_frameBuffer.
    static constexpr FwSizeType ASM_MAX_SIZE = static_cast<FwSizeType>(AsmFramerCfg::AsmMaxSize);

    //! Size in bytes of the default ASM (CCSDS 131.0-B-5 Section 9.3.1)
    static constexpr FwSizeType ASM_DEFAULT_SIZE = 4;

    //! Smallest enabled ASM in bytes: the shortest CCSDS 131.0-B-5 pattern (32 bits). Shorter
    //! markers false-sync on random channel data at 2^-(8 * size) per byte position, so they are
    //! rejected; the standard lengths are 4, 8, 12, 16 and 24 bytes (24 requires raising
    //! Svc.AsmFramerCfg.AsmMaxSize).
    static constexpr FwSizeType ASM_MIN_SIZE = 4;

    //! Largest transfer frame accepted on dataIn (Svc.AsmFramerCfg.MaxFrameSize, by default the
    //! AOS bound so the component can follow either upstream framer)
    static constexpr FwSizeType FRAME_MAX_SIZE = static_cast<FwSizeType>(AsmFramerCfg::MaxFrameSize);

  private:
    enum class BufferOwnershipState {
        NOT_OWNED,  //!< The frame buffer is currently not owned by the AsmFramer
        OWNED,      //!< The frame buffer is currently owned by the AsmFramer
    };

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct AsmFramer object. The ASM defaults to the 32-bit pattern
    //! 0x1ACFFC1D (CCSDS 131.0-B-5 Section 9.3.1)
    explicit AsmFramer(const char* const compName  //!< The component name
    );

    //! Destroy AsmFramer object
    ~AsmFramer();

    //! Component instances are constructed once by the topology and never copied
    AsmFramer(const AsmFramer&) = delete;
    AsmFramer& operator=(const AsmFramer&) = delete;

    //! Configure the ASM pattern. Sizes ASM_MIN_SIZE to ASM_MAX_SIZE select a marker such as
    //! the coded-data patterns of CCSDS 131.0-B-5 Sections 9.3.2 - 9.3.5; size 0
    //! disables the marker so frames pass through unchanged. Coded-data patterns are
    //! only appropriate when the frames on dataIn are already encoded (the standard
    //! attaches them to the codeword, Section 9.4.1). Must be called during topology
    //! setup, before any frame flows; the ownership assertion is a guard against misuse,
    //! not a runtime reconfiguration path.
    void configure(
        const Fw::ConstByteArray& asmPattern  //!< ASM pattern bytes; size 0 or in [ASM_MIN_SIZE, ASM_MAX_SIZE]
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
    //! Port to receive a transfer frame to sync-mark, in a Fw::Buffer with
    //! optional context. Emits the Sync-Marked Transfer Frame (ASM + frame)
    //! on dataOut (CCSDS 131.0-B-5 Section 9.4)
    void dataIn_handler(FwIndexType portNum,  //!< The port number
                        Fw::Buffer& data,
                        const ComCfg::FrameContext& context) override;

    //! Handler implementation for dataReturnIn
    //!
    //! Buffer coming from a deallocate call in a ComDriver component
    void dataReturnIn_handler(FwIndexType portNum,  //!< The port number
                              Fw::Buffer& frameBuffer,
                              const ComCfg::FrameContext& context) override;

    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The configured ASM pattern
    U8 m_asm[ASM_MAX_SIZE];

    //! The configured ASM size in bytes (0 when the marker is disabled)
    FwSizeType m_asmSize;

    //! Internal buffer holding the Sync-Marked Transfer Frame: the largest
    //! supported ASM plus the largest frame either upstream framer can emit
    U8 m_frameBuffer[ASM_MAX_SIZE + FRAME_MAX_SIZE];

    //! Ownership state of the internal frame buffer
    BufferOwnershipState m_bufferState = BufferOwnershipState::OWNED;

    //! Upstream frame buffer held until the Sync-Marked Transfer Frame buffer is returned on dataReturnIn
    Fw::Buffer m_upstreamBuffer;

    //! Context of the held upstream frame
    ComCfg::FrameContext m_upstreamContext;
};

}  // namespace Ccsds

}  // namespace Svc

#endif
