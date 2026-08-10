// ======================================================================
// \title  AsmFramer.hpp
// \author devin
// \brief  hpp file for AsmFramer component implementation class
// ======================================================================

#ifndef Svc_Ccsds_AsmFramer_HPP
#define Svc_Ccsds_AsmFramer_HPP

#include "Svc/Ccsds/AsmFramer/AsmFramerComponentAc.hpp"
#include "config/FppConstantsAc.hpp"

namespace Svc {

namespace Ccsds {

class AsmFramer final : public AsmFramerComponentBase {
    friend class AsmFramerTester;

  public:
    //! Maximum supported ASM size in bytes: 128 bits, the longest pattern
    //! defined by CCSDS 131.0-B-5 Section 9.3 (rate-1/6 Turbo is 192 bits,
    //! rate-1/4 Turbo is 128 bits; sizes above rate-1/4 use 128 bits or less)
    static constexpr FwSizeType ASM_MAX_SIZE = 16;

    //! Size in bytes of the default ASM (CCSDS 131.0-B-5 Section 9.3.1)
    static constexpr FwSizeType ASM_DEFAULT_SIZE = 4;

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
    AsmFramer(const char* const compName  //!< The component name
    );

    //! Destroy AsmFramer object
    ~AsmFramer();

    //! Configure a non-default ASM pattern (CCSDS 131.0-B-5 Sections 9.3.2 - 9.3.4).
    //! Must be called before the component receives data, and may not be called
    //! while the component holds outstanding frames.
    void configure(const U8* asmPattern,  //!< ASM pattern bytes
                   FwSizeType asmSize     //!< Size of the ASM pattern in bytes [1, ASM_MAX_SIZE]
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

    //! The configured ASM size in bytes
    FwSizeType m_asmSize;

    //! Internal buffer holding the Sync-Marked Transfer Frame
    U8 m_frameBuffer[ASM_MAX_SIZE + ComCfg::TmFrameFixedSize];

    //! Ownership state of the internal frame buffer
    BufferOwnershipState m_bufferState = BufferOwnershipState::OWNED;
};

}  // namespace Ccsds

}  // namespace Svc

#endif
