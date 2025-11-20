// ======================================================================
// \title  ByteStreamBufferAdapterTester.hpp
// \author shahab
// \brief  hpp file for ByteStreamBufferAdapter component test harness implementation class
// ======================================================================

#ifndef Drv_ByteStreamBufferAdapterTester_HPP
#define Drv_ByteStreamBufferAdapterTester_HPP

#include "Drv/ByteStreamBufferAdapter/ByteStreamBufferAdapter.hpp"
#include "Drv/ByteStreamBufferAdapter/ByteStreamBufferAdapterGTestBase.hpp"

namespace Drv {

class ByteStreamBufferAdapterTester final : public ByteStreamBufferAdapterGTestBase {
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

    //! Construct object ByteStreamBufferAdapterTester
    ByteStreamBufferAdapterTester();

    //! Destroy object ByteStreamBufferAdapterTester
    ~ByteStreamBufferAdapterTester();

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
    ByteStreamBufferAdapter component;
};

}  // namespace Drv

#endif
