// ======================================================================
// \title  CcsdsSdlsDeframerTester.hpp
// \author lestarch-autobot
// \brief  hpp file for CcsdsSdlsDeframer component test harness implementation class
// ======================================================================

#ifndef Svc_Ccsds_CcsdsSdlsDeframerTester_HPP
#define Svc_Ccsds_CcsdsSdlsDeframerTester_HPP

#include "Svc/Ccsds/CcsdsSdlsDeframer/CcsdsSdlsDeframer.hpp"
#include "Svc/Ccsds/CcsdsSdlsDeframer/CcsdsSdlsDeframerGTestBase.hpp"
#include "TestUtils/RuleBasedTesting.hpp"

namespace Svc {

namespace Ccsds {

class CcsdsSdlsDeframerTester : public CcsdsSdlsDeframerGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    //! Maximum size of histories storing events, telemetry, and port outputs
    static const FwSizeType MAX_HISTORY_SIZE = 10;

    //! Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

    //! Maximum size of a test frame buffer
    static const FwSizeType TEST_BUFFER_SIZE = 64;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object CcsdsSdlsDeframerTester
    CcsdsSdlsDeframerTester();

    //! Destroy object CcsdsSdlsDeframerTester
    ~CcsdsSdlsDeframerTester();

  private:
    // ----------------------------------------------------------------------
    // Handler overrides for typed from ports
    // ----------------------------------------------------------------------

    //! Override recording the request and returning the staged status
    Svc::Ccsds::SdlsStatus from_decryptOut_handler(FwIndexType portNum,
                                                   U16 securityAssociationIndex,
                                                   Fw::Buffer& data,
                                                   const ComCfg::FrameContext& context) override;

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
    CcsdsSdlsDeframer component;

    //! Status returned by the decryption helper stub (from_decryptOut)
    Svc::Ccsds::SdlsStatus m_decryptStatus = Svc::Ccsds::SdlsStatus::SUCCESS;

    //! Copy of the data forwarded on the last from_decryptOut invocation
    U8 m_lastDecryptData[TEST_BUFFER_SIZE];

    //! Size of the data forwarded on the last from_decryptOut invocation
    FwSizeType m_lastDecryptSize = 0;

  public:
    // ----------------------------------------------------------------------
    // Rule Based Testing
    // ----------------------------------------------------------------------

    //! Rules for the deframing path (dataIn)
    FW_RBT_DEFINE_RULE(CcsdsSdlsDeframerTester, Deframe, Nominal);
    FW_RBT_DEFINE_RULE(CcsdsSdlsDeframerTester, Deframe, InsufficientLength);
    FW_RBT_DEFINE_RULE(CcsdsSdlsDeframerTester, Deframe, DecryptFailure);

    //! Rules for the decrypted data and ownership return paths
    FW_RBT_DEFINE_RULE(CcsdsSdlsDeframerTester, DataFlow, DecryptedData);
    FW_RBT_DEFINE_RULE(CcsdsSdlsDeframerTester, DataFlow, DataReturn);
    FW_RBT_DEFINE_RULE(CcsdsSdlsDeframerTester, DataFlow, BufferReturn);
};

}  // namespace Ccsds

}  // namespace Svc

#endif
