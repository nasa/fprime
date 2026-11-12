// ======================================================================
// \title  AsmFramerTester.hpp
// \author devin
// \brief  hpp file for AsmFramer component test harness implementation class
// ======================================================================

#ifndef Svc_Ccsds_AsmFramerTester_HPP
#define Svc_Ccsds_AsmFramerTester_HPP

#include "Svc/Ccsds/AsmFramer/AsmFramer.hpp"
#include "Svc/Ccsds/AsmFramer/AsmFramerGTestBase.hpp"

namespace Svc {

namespace Ccsds {

class AsmFramerTester final : public AsmFramerGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    // Maximum size of histories storing events, telemetry, and port outputs
    static const FwSizeType MAX_HISTORY_SIZE = 10;

    // Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object AsmFramerTester
    AsmFramerTester();

    //! Destroy object AsmFramerTester
    ~AsmFramerTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    void testComStatusPassthrough();
    void testNominalFraming();
    void testConfiguredAsm();
    void testDisabledAsm();
    void testConfigureInvalid();
    void testFrameSizeBounds();
    void testDataReturn();
    void testBufferOwnershipState();
    void testReentrantReturn();

  private:
    // ----------------------------------------------------------------------
    // Handler overrides
    // ----------------------------------------------------------------------

    //! Records the returned upstream frame; when armed, first re-enters dataIn with a new
    //! frame (as AosFramer does when its buffer returns) before recording
    void from_dataReturnOut_handler(FwIndexType portNum,
                                    Fw::Buffer& data,
                                    const ComCfg::FrameContext& context) override;

    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Connect ports
    void connectPorts();

    //! Initialize components
    void initComponents();

    //! Send a frame through dataIn and verify the SMTF emitted on dataOut (ASM followed by
    //! the unmodified frame, context passed through) and the input buffer returned on
    //! dataReturnOut, at history index `index`. Reclaims the internal buffer afterwards.
    void sendAndVerifyFrame(const Fw::ConstByteArray& expectedAsm,  //!< Expected marker (size 0 when disabled)
                            Fw::Buffer& frame,                      //!< The frame to send
                            const ComCfg::FrameContext& context,    //!< The context to send
                            U32 index                               //!< Expected history index of the output
    );

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    AsmFramer component;

    //! vcId offset applied to the frame emitted from inside from_dataReturnOut_handler
    static constexpr U8 REENTRANT_VCID_OFFSET = 10;

    //! When true, from_dataReturnOut_handler re-enters dataIn once before recording
    bool m_reenterOnReturn = false;

    //! Storage for the frame sent on re-entry
    U8 m_reentrantData[16] = {};
};

}  // namespace Ccsds

}  // namespace Svc

#endif
