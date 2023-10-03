// ======================================================================
// \title  StreamCrossover.hpp
// \author ethanchee
// \brief  cpp file for StreamCrossover test harness implementation class
// ======================================================================

#include "StreamCrossoverTester.hpp"

namespace Drv {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  StreamCrossoverTester ::
    StreamCrossoverTester() :
      StreamCrossoverGTestBase("Tester", StreamCrossoverTester::MAX_HISTORY_SIZE),
      component("StreamCrossover")
  {
    this->initComponents();
    this->connectPorts();
  }

  StreamCrossoverTester ::
    ~StreamCrossoverTester()
  {

  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  void StreamCrossoverTester ::
    sendTestBuffer()
  {
    U8 testStr[6] = "test\n";
    Fw::Buffer sendBuffer(testStr, sizeof(testStr));
    this->invoke_to_streamIn(0, sendBuffer, Drv::RecvStatus::RECV_OK);

    // Ensure only one buffer was sent to streamOut
    ASSERT_from_streamOut_SIZE(1);

    // Ensure the sendBuffer was sent
    ASSERT_from_streamOut(0, sendBuffer);
  }

  void StreamCrossoverTester ::
    testFail()
  {
    U8 testStr[6] = "test\n";
    Fw::Buffer sendBuffer(testStr, sizeof(testStr));
    this->invoke_to_streamIn(0, sendBuffer, Drv::RecvStatus::RECV_ERROR);

    // Ensure only one buffer was sent to errorDeallocate port on RECV_ERROR
    ASSERT_from_errorDeallocate_SIZE(1);

    // Ensure the sendBuffer was sent
    ASSERT_from_errorDeallocate(0, sendBuffer);

    // Ensure the error event was sent
    ASSERT_EVENTS_StreamOutError_SIZE(1);

    // Ensure the error is SEND_ERROR
    ASSERT_EVENTS_StreamOutError(0, Drv::SendStatus::SEND_ERROR);
  }

  // ----------------------------------------------------------------------
  // Handlers for typed from ports
  // ----------------------------------------------------------------------

  Drv::SendStatus StreamCrossoverTester ::
    from_streamOut_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &sendBuffer
    )
  {
    this->pushFromPortEntry_streamOut(sendBuffer);

    U8 testStr[6] = "test\n";
    Fw::Buffer cmpBuffer(testStr, sizeof(testStr));

    if(!(cmpBuffer == sendBuffer))
    {
      return Drv::SendStatus::SEND_ERROR;
    }

    return Drv::SendStatus::SEND_OK;
  }

  void StreamCrossoverTester ::
    from_errorDeallocate_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    this->pushFromPortEntry_errorDeallocate(fwBuffer);
  }


} // end namespace Drv
