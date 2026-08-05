// ======================================================================
// \title  CcsdsSdlsFramerTester.hpp
// \author devin
// \brief  hpp file for CcsdsSdlsFramer component test harness implementation class
// ======================================================================

#ifndef Svc_Ccsds_CcsdsSdlsFramerTester_HPP
#define Svc_Ccsds_CcsdsSdlsFramerTester_HPP

#include "Svc/Ccsds/CcsdsSdlsFramer/CcsdsSdlsFramer.hpp"
#include "Svc/Ccsds/CcsdsSdlsFramer/CcsdsSdlsFramerGTestBase.hpp"
#include "TestUtils/RuleBasedTesting.hpp"

namespace Svc {

namespace Ccsds {

class CcsdsSdlsFramerTester : public CcsdsSdlsFramerGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    //! Maximum size of histories storing events, telemetry, and port outputs
    static const FwSizeType MAX_HISTORY_SIZE = 10;

    //! Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

    //! Maximum size of a test data buffer
    static const FwSizeType TEST_BUFFER_SIZE = 64;

    //! Size of the allocation storage (data + SA index)
    static const FwSizeType TEST_ALLOCATION_SIZE = TEST_BUFFER_SIZE + sizeof(U16);

    //! Security association index staged into the SA_INDEX parameter
    static const U16 TEST_PARAM_SA_INDEX = 27;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object CcsdsSdlsFramerTester
    CcsdsSdlsFramerTester();

    //! Destroy object CcsdsSdlsFramerTester
    ~CcsdsSdlsFramerTester();

  private:
    // ----------------------------------------------------------------------
    // Handler overrides for typed from ports
    // ----------------------------------------------------------------------

    //! Override recording the encryption request
    void from_encryptOut_handler(FwIndexType portNum,
                                 U16 securityAssociationIndex,
                                 Fw::Buffer& data,
                                 const ComCfg::FrameContext& context) override;

    //! Override returning a buffer from the tester-owned allocation storage
    Fw::Buffer from_bufferAllocate_handler(FwIndexType portNum, FwSizeType size) override;

  private:
    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Connect ports
    void connectPorts();

    //! Initialize components
    void initComponents();

  public:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    CcsdsSdlsFramer component;

    //! Whether the allocation stub returns an undersized buffer
    bool m_allocateUndersized = false;

    //! Whether the allocation stub returns an invalid buffer
    bool m_allocateInvalid = false;

    //! Storage backing the allocation stub
    U8 m_allocationStorage[TEST_ALLOCATION_SIZE] = {};

  public:
    // ----------------------------------------------------------------------
    // Rule Based Testing
    // ----------------------------------------------------------------------

    //! Rules for the framing path (dataIn)
    FW_RBT_DEFINE_RULE(CcsdsSdlsFramerTester, Frame, ContextSa);
    FW_RBT_DEFINE_RULE(CcsdsSdlsFramerTester, Frame, ParameterSa);
    FW_RBT_DEFINE_RULE(CcsdsSdlsFramerTester, Frame, EncryptFailure);

    //! Rules for the encrypted data and ownership return paths
    FW_RBT_DEFINE_RULE(CcsdsSdlsFramerTester, DataFlow, EncryptedData);
    FW_RBT_DEFINE_RULE(CcsdsSdlsFramerTester, DataFlow, AllocationFailure);
    FW_RBT_DEFINE_RULE(CcsdsSdlsFramerTester, DataFlow, DataReturn);
    FW_RBT_DEFINE_RULE(CcsdsSdlsFramerTester, DataFlow, BufferReturn);
    FW_RBT_DEFINE_RULE(CcsdsSdlsFramerTester, DataFlow, ComStatus);
};

}  // namespace Ccsds

}  // namespace Svc

#endif
