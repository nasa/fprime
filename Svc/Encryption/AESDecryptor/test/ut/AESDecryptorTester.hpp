// ======================================================================
// \title  AESDecryptorTester.hpp
// \author vivi
// \brief  hpp file for AESDecryptor component test harness implementation class
// ======================================================================

#ifndef Svc_Ccsds_AESDecryptorTester_HPP
#define Svc_Ccsds_AESDecryptorTester_HPP

#include "Svc/Encryption/AESDecryptor/AESDecryptor.hpp"
#include "Svc/Encryption/AESDecryptor/AESDecryptorGTestBase.hpp"

namespace Svc {

namespace Ccsds {

class AESDecryptorTester final : public AESDecryptorGTestBase {
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

    //! Construct object AESDecryptorTester
    AESDecryptorTester();

    //! Destroy object AESDecryptorTester
    ~AESDecryptorTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! To do
    void toDo();

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
    AESDecryptor component;
};

}  // namespace Ccsds

}  // namespace Svc

#endif
