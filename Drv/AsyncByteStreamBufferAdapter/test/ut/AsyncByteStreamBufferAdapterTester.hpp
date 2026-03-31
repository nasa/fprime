// ======================================================================
// \title  AsyncByteStreamBufferAdapterTester.hpp
// \author shahab
// \brief  hpp file for AsyncByteStreamBufferAdapter component test harness implementation class
// ======================================================================

#ifndef Drv_AsyncByteStreamBufferAdapterTester_HPP
#define Drv_AsyncByteStreamBufferAdapterTester_HPP

#include "Drv/AsyncByteStreamBufferAdapter/AsyncByteStreamBufferAdapter.hpp"
#include "Drv/AsyncByteStreamBufferAdapter/AsyncByteStreamBufferAdapterGTestBase.hpp"

// Larger than com buffer size
#define DATA_SIZE (FW_COM_BUFFER_MAX_SIZE * 10 + sizeof(U32) + sizeof(U32) + sizeof(FwBuffSizeType))

namespace Drv {

class AsyncByteStreamBufferAdapterTester final : public AsyncByteStreamBufferAdapterGTestBase {
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

    //! Construct object AsyncByteStreamBufferAdapterTester
    AsyncByteStreamBufferAdapterTester();

    //! Destroy object AsyncByteStreamBufferAdapterTester
    ~AsyncByteStreamBufferAdapterTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //!
    void test_byte_stream_out();

    //!
    void test_byte_stream_in();

    void test_byte_stream_return();

    void from_toByteStreamDriver_handler(FwIndexType portNum, Fw::Buffer& fwBuffer) override;

  private:
    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Connect ports
    void connectPorts();

    //! Initialize components
    void initComponents();

    void random_fill(Fw::SerializeBufferBase& buffer, U32 max_size);

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    AsyncByteStreamBufferAdapter component;
    Fw::Buffer m_buffer;
    U8 m_data_store[DATA_SIZE];
    bool m_byte_stream_driver_fail;
};

}  // namespace Drv

#endif
