// ======================================================================
// \title  AESEncryptorTester.hpp
// \author vivi
// \brief  hpp file for AESEncryptor component test harness implementation class
// ======================================================================

#ifndef Svc_Ccsds_AESEncryptorTester_HPP
#define Svc_Ccsds_AESEncryptorTester_HPP

#include "Svc/Encryption/AESEncryptor/AESEncryptor.hpp"
#include "Svc/Encryption/AESEncryptor/AESEncryptorGTestBase.hpp"

namespace Svc {

namespace Ccsds {

class AESEncryptorTester final : public AESEncryptorGTestBase {
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

    //! Construct object AESEncryptorTester
    AESEncryptorTester();

    //! Destroy object AESEncryptorTester
    ~AESEncryptorTester();

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
    AESEncryptor component;
};

}  // namespace Ccsds

}  // namespace Svc

#endif
