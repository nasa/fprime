// ======================================================================
// \title  ByteStreamBufferAdapterTester.hpp
// \author shahab
// \brief  hpp file for ByteStreamBufferAdapter component test harness implementation class
// ======================================================================

#ifndef Drv_ByteStreamBufferAdapterTester_HPP
#define Drv_ByteStreamBufferAdapterTester_HPP

#include "Drv/ByteStreamBufferAdapter/ByteStreamBufferAdapter.hpp"
#include "Drv/ByteStreamBufferAdapter/ByteStreamBufferAdapterGTestBase.hpp"

// Larger than com buffer size
#define DATA_SIZE (FW_COM_BUFFER_MAX_SIZE * 10 + sizeof(U32) + sizeof(U32) + sizeof(FwBuffSizeType))

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

    //! Test buffer streaming out through the ByteStreamDriver
    void test_byte_stream_out();

    //! Test buffer streaming in from the ByteStreamDriver
    void test_byte_stream_in();

    //! Test buffer return handling from bufferOutReturn port
    void test_byte_stream_return();

    //! Test driver ready state behavior specifically
    void test_driver_ready_state();

    Drv::ByteStreamStatus from_toByteStreamDriver_handler(FwIndexType portNum, Fw::Buffer& sendBuffer) override;

  private:
    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Connect ports
    void connectPorts();

    //! Initialize components
    void initComponents();

    //! Fill buffer with random data
    void random_fill(Fw::SerializeBufferBase& buffer, U32 max_size);

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    ByteStreamBufferAdapter component;

    //! Buffer for testing
    Fw::Buffer m_buffer;

    //! Storage for buffer data
    U8 m_data_store[DATA_SIZE];

    //! Flag to control error conditions in byte stream driver handler
    bool m_byte_stream_driver_fail;
};

}  // namespace Drv

#endif
