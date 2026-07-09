// ======================================================================
// \title  SdlsFileKeyManagerTester.hpp
// \author lestarch-autobot
// \brief  hpp file for SdlsFileKeyManager component test harness implementation class
// ======================================================================

#ifndef Svc_Ccsds_SdlsFileKeyManagerTester_HPP
#define Svc_Ccsds_SdlsFileKeyManagerTester_HPP

#include "Svc/Ccsds/SdlsFileKeyManager/SdlsFileKeyManager.hpp"
#include "Svc/Ccsds/SdlsFileKeyManager/SdlsFileKeyManagerGTestBase.hpp"

namespace Svc {

namespace Ccsds {

class SdlsFileKeyManagerTester : public SdlsFileKeyManagerGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    //! Maximum size of histories storing events, telemetry, and port outputs
    static const FwSizeType MAX_HISTORY_SIZE = 10;

    //! Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object SdlsFileKeyManagerTester
    SdlsFileKeyManagerTester();

    //! Destroy object SdlsFileKeyManagerTester
    ~SdlsFileKeyManagerTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Test a nominal key read of a random length in (0, MAX_SDLS_KEY_SIZE]
    void testNominalRead();

    //! Test a key request against a missing file
    void testMissingFile();

    //! Test a key request against a file shorter than the configured key length
    void testShortFile();

  private:
    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Write a key file of the given size filled with random data, recorded in m_keyData
    void writeKeyFile(const char* path, FwSizeType size);

    //! Connect ports
    void connectPorts();

    //! Initialize components
    void initComponents();

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    SdlsFileKeyManager component;

    //! Data written to the key file
    U8 m_keyData[SdlsCfg::MAX_SDLS_KEY_SIZE];
};

}  // namespace Ccsds

}  // namespace Svc

#endif
