// ======================================================================
// \title  ComQueue/test/ut/Tester.hpp
// \author vbai
// \brief  hpp file for ComQueue test harness implementation class
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "ComQueueGTestBase.hpp"
#include "Fw/Com/ComPacket.hpp"
#include "Svc/ComQueue/ComQueue.hpp"

#define BUFFER_LENGTH 7u
#define BUFFER_DATA {0x00, 0x00, 0x00, 0x01, 0xad, 0xbe, 0xde}  // First 4 bytes are the ComPacketType
#define BUFFER_DATA_OFFSET sizeof(Fw::ComPacketType)

namespace Svc {

class ComQueueTester : public ComQueueGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    // Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

    // Queue depth supplied to the component instance under test
    static const FwSizeType TEST_INSTANCE_QUEUE_DEPTH = 10;

  private:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

  public:
    //! Construct object ComQueueTester
    //!
    ComQueueTester();

    //! Destroy object ComQueueTester
    //!
    ~ComQueueTester();

    //! Dispatch all component messages
    //!
    void dispatchAll();

  public:
    // ----------------------------------------------------------------------
    // Helpers
    // ----------------------------------------------------------------------
    void configure();

    void sendByQueueNumber(Fw::Buffer& buffer, FwIndexType queueNumber, FwIndexType& portNum, QueueType& queueType);

    void emitOne();

    void emitOneAndCheck(FwIndexType expectedIndex, U8* expectedData, FwSizeType expectedDataSize);

    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    void testQueueSend();

    void testQueuePause();

    void testPrioritySend();

    void testExternalQueueOverflow();

    void testInternalQueueOverflow();

    void testReadyFirst();

    void testContextData();

    void testBufferQueueReturn();

  private:
    // ----------------------------------------------------------------------
    // Helper methods
    // ----------------------------------------------------------------------

    //! Connect ports
    //!
    void connectPorts();

    //! Initialize components
    //!
    void initComponents();

  private:
    // ----------------------------------------------------------------------
    // Variables
    // ----------------------------------------------------------------------

    //! The component under test
    //!
    ComQueue component;
};

}  // end namespace Svc

#endif
