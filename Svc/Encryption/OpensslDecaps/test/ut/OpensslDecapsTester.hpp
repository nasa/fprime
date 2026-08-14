// ======================================================================
// \title  OpensslDecapsTester.hpp
// \author vivi
// \brief  hpp file for OpensslDecaps component test harness implementation class
// ======================================================================

#ifndef Svc_Ccsds_OpensslDecapsTester_HPP
#define Svc_Ccsds_OpensslDecapsTester_HPP

#include "Svc/Encryption/OpensslDecaps/OpensslDecaps.hpp"
#include "Svc/Encryption/OpensslDecaps/OpensslDecapsGTestBase.hpp"

namespace Svc {

namespace Ccsds {

class OpensslDecapsTester final : public OpensslDecapsGTestBase {
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

    //! Construct object OpensslDecapsTester
    OpensslDecapsTester();

    //! Destroy object OpensslDecapsTester
    ~OpensslDecapsTester();

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
    OpensslDecaps component;
};

}  // namespace Ccsds

}  // namespace Svc

#endif
