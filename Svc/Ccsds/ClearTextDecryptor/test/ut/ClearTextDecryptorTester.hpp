// ======================================================================
// \title  ClearTextDecryptorTester.hpp
// \author lestarch-autobot
// \brief  hpp file for ClearTextDecryptor component test harness implementation class
// ======================================================================

#ifndef Svc_Ccsds_ClearTextDecryptorTester_HPP
#define Svc_Ccsds_ClearTextDecryptorTester_HPP

#include "Svc/Ccsds/ClearTextDecryptor/ClearTextDecryptor.hpp"
#include "Svc/Ccsds/ClearTextDecryptor/ClearTextDecryptorGTestBase.hpp"

namespace Svc {

namespace Ccsds {

class ClearTextDecryptorTester : public ClearTextDecryptorGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    //! Maximum size of histories storing events, telemetry, and port outputs
    static const FwSizeType MAX_HISTORY_SIZE = 10;

    //! Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

    //! Size of each test buffer
    static const FwSizeType TEST_BUFFER_SIZE = 64;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object ClearTextDecryptorTester
    ClearTextDecryptorTester();

    //! Destroy object ClearTextDecryptorTester
    ~ClearTextDecryptorTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Test a decryptIn request passes buffer and context through unmodified with SUCCESS
    void testDecryptPassThrough();

    //! Test decryptReturnIn ownership returns pass through to bufferReturnOut
    void testReturnPassThrough();

  private:
    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Connect ports
    void connectPorts();

    //! Initialize components
    void initComponents();

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    ClearTextDecryptor component;
};

}  // namespace Ccsds

}  // namespace Svc

#endif
