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

    //! 
    void test_byte_stream_out();

    //! 
    void test_byte_stream_in();

    void test_byte_stream_return();

    Drv::ByteStreamStatus from_toByteStreamDriver_handler(
        FwIndexType portNum,
        Fw::Buffer& sendBuffer) override;

    void textLogIn(FwEventIdType id, 
        const Fw::Time& timeTag,        
        const Fw::LogSeverity severity,
        const Fw::TextLogString& text) override; 

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
    ByteStreamBufferAdapter component;
    Fw::Buffer m_buffer;
    U8 m_data_store[DATA_SIZE];
    bool m_byte_stream_driver_fail; 
};

}  // namespace Drv

#endif
