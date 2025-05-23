// ======================================================================
// \title  TMFramer.hpp
// \author thomas-bc
// \brief  hpp file for TMFramer component implementation class
// ======================================================================

#ifndef Svc_CCSDS_TMFramer_HPP
#define Svc_CCSDS_TMFramer_HPP

#include "Svc/CCSDS/TMFramer/TMFramerComponentAc.hpp"

namespace Svc {

namespace CCSDS {

class TMFramer final : public TMFramerComponentBase {
  friend class TMFramerTester;

  static constexpr U8 IDLE_DATA_PATTERN = 0x44;

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct TMFramer object
    TMFramer(const char* const compName  //!< The component name
    );

    //! Destroy TMFramer object
    ~TMFramer();

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
    //! TODO: mention context values that are being used 
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
    //! Fill the frame buffer with an Idle Packet to complete the frame data field
    //! as per CCSDS TM Protocol paragraph 4.2.2.5. Idle packet is inserted at the
    //! start_index index of the frame buffer, and fills it up to the end minus CRC
    // void fill_with_idle_packet(U16 start_index);
    void fill_with_idle_packet(Fw::SerializeBufferBase& serializer);

    // Because the TM protocol use fixed width frames, and only one frame is in transit between ComQueue and
    // ComInterface at a time, we can use a member fixed-size buffer to hold the frame data
    U8 m_frameBuffer[ComCfg::FppConstant_TmFrameFixedSize::TmFrameFixedSize];  //!< Buffer to hold the frame data

    // Current implementation uses a single virtual channel, so we can use a single virtual frame count
    U8 m_masterFrameCount;   //!< Master Frame Count - 8 bits - wraps around at 255
    U8 m_virtualFrameCount;  //!< Virtual Frame Count - 8 bits - wraps around at 255
};

}  // namespace CCSDS

}  // namespace Svc

#endif
