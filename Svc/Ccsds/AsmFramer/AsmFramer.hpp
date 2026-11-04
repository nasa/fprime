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
    //! Maximum supported ASM size in bytes: 128 bits, the rate-1/4 Turbo pattern
    //! (CCSDS 131.0-B-5 Section 9.3.2). This is an implementation bound, not a
    //! standard one: the 192-bit rate-1/6 Turbo ASM is intentionally not supported.
    //! Raise this constant (and thereby m_frameBuffer) to add it.
    static constexpr FwSizeType ASM_MAX_SIZE = 16;

    //! Size in bytes of the default ASM (CCSDS 131.0-B-5 Section 9.3.1)
    static constexpr FwSizeType ASM_DEFAULT_SIZE = 4;

    //! Largest transfer frame accepted on dataIn: the larger of the fixed TM frame
    //! and the maximum AOS frame, so the component can follow either upstream framer
    static constexpr FwSizeType FRAME_MAX_SIZE =
        (static_cast<FwSizeType>(ComCfg::AosMaxFrameFixedSize) > static_cast<FwSizeType>(ComCfg::TmFrameFixedSize))
            ? static_cast<FwSizeType>(ComCfg::AosMaxFrameFixedSize)
            : static_cast<FwSizeType>(ComCfg::TmFrameFixedSize);

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

    //! Configure the ASM pattern. Sizes 1 to ASM_MAX_SIZE select a marker such as
    //! the coded-data patterns of CCSDS 131.0-B-5 Sections 9.3.2 - 9.3.4; size 0
    //! disables the marker so frames pass through unchanged. Must be called before
    //! the component receives data, and may not be called while the component holds
    //! outstanding frames.
    void configure(const Fw::ConstByteArray& asmPattern  //!< ASM pattern bytes; size in [0, ASM_MAX_SIZE]
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
};

}  // namespace Ccsds

}  // namespace Svc

#endif
