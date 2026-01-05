// ======================================================================
// \title  CfdpManagerTester.hpp
// \author campuzan
// \brief  hpp file for CfdpManager component test harness implementation class
// ======================================================================

#ifndef Svc_Ccsds_CfdpManagerTester_HPP
#define Svc_Ccsds_CfdpManagerTester_HPP

#include "Svc/Ccsds/CfdpManager/CfdpManager.hpp"
#include "Svc/Ccsds/CfdpManager/CfdpManagerGTestBase.hpp"

namespace Svc {

namespace Ccsds {

class CfdpManagerTester final : public CfdpManagerGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    // Maximum size of histories storing events, telemetry, and port outputs
    static const FwSizeType MAX_HISTORY_SIZE = 10;

    // Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

    // Queue depth supplied to the component instance under test
    static const FwSizeType TEST_INSTANCE_QUEUE_DEPTH = 10;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object CfdpManagerTester
    CfdpManagerTester();

    //! Destroy object CfdpManagerTester
    ~CfdpManagerTester();

  public:
    // ----------------------------------------------------------------------
    // White Box PDU Tests
    // ----------------------------------------------------------------------

    //! Test generating a Metadata PDU
    void testMetaDataPdu();

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
    CfdpManager component;
};

}  // namespace Ccsds

}  // namespace Svc

#endif
