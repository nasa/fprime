// ======================================================================
// \title  ClearTextEncryptorTester.hpp
// \author lestarch-autobot
// \brief  hpp file for ClearTextEncryptor component test harness implementation class
// ======================================================================

#ifndef Svc_Ccsds_ClearTextEncryptorTester_HPP
#define Svc_Ccsds_ClearTextEncryptorTester_HPP

#include "Svc/Ccsds/ClearTextEncryptor/ClearTextEncryptor.hpp"
#include "Svc/Ccsds/ClearTextEncryptor/ClearTextEncryptorGTestBase.hpp"

namespace Svc {

namespace Ccsds {

class ClearTextEncryptorTester : public ClearTextEncryptorGTestBase {
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

    //! Construct object ClearTextEncryptorTester
    ClearTextEncryptorTester();

    //! Destroy object ClearTextEncryptorTester
    ~ClearTextEncryptorTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Test an encryptIn request passes buffer and context through unmodified with SUCCESS
    void testEncryptPassThrough();

    //! Test encryptReturnIn ownership returns pass through to bufferReturnOut
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
    ClearTextEncryptor component;
};

}  // namespace Ccsds

}  // namespace Svc

#endif
