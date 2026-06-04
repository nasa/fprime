// ======================================================================
// \title  ComBufferToComAdapterTester.hpp
// \brief  hpp file for ComBufferToComAdapter component test harness
// ======================================================================

#ifndef Svc_ComBufferToComAdapterTester_HPP
#define Svc_ComBufferToComAdapterTester_HPP

#include "Svc/ComBufferToComAdapter/ComBufferToComAdapter.hpp"
#include "Svc/ComBufferToComAdapter/ComBufferToComAdapterGTestBase.hpp"

namespace Svc {

class ComBufferToComAdapterTester : public ComBufferToComAdapterGTestBase {
  public:
    // Maximum size of histories storing events, telemetry, and port outputs
    static const FwSizeType MAX_HISTORY_SIZE = 10;
    // Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object ComBufferToComAdapterTester
    ComBufferToComAdapterTester();

    //! Destroy object ComBufferToComAdapterTester
    ~ComBufferToComAdapterTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Nominal: APID is prepended as a descriptor to the legacy buffer
    void testNominal();

    //! A zero-length payload yields a buffer containing only the descriptor
    void testEmptyPayload();

  private:
    // ----------------------------------------------------------------------
    // Handlers for typed from ports
    // ----------------------------------------------------------------------

    //! Handler for from_comOut
    void from_comOut_handler(FwIndexType portNum,  //!< The port number
                             Fw::ComBuffer& data,  //!< Buffer containing packet data
                             U32 context           //!< Call context value
                             ) override;

  private:
    // ----------------------------------------------------------------------
    // Helper methods
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
    ComBufferToComAdapter component;
};

}  // namespace Svc

#endif
