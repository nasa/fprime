// ======================================================================
// \title  StreamCrossover.hpp
// \author ethanchee
// \brief  cpp file for StreamCrossover test harness implementation class
// ======================================================================

#include "Tester.hpp"

namespace Drv {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  Tester ::
    Tester() :
      StreamCrossoverGTestBase("Tester", Tester::MAX_HISTORY_SIZE),
      component("StreamCrossover")
  {
    this->initComponents();
    this->connectPorts();
  }

  Tester ::
    ~Tester()
  {

  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  void Tester ::
    sendTestBuffer()
  {
    this->clearHistory();

    U8 testStr[6] = "test\n";
    Fw::Buffer sendBuffer(testStr, sizeof(testStr));
    this->invoke_to_streamIn(0, sendBuffer, Drv::RecvStatus::RECV_OK);
  }

  // ----------------------------------------------------------------------
  // Handlers for typed from ports
  // ----------------------------------------------------------------------

  Drv::SendStatus Tester ::
    from_streamOut_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &sendBuffer
    )
  {
    this->pushFromPortEntry_streamOut(sendBuffer);

    // Ensure only one buffer was sent to streamOut
    ASSERT_from_streamOut_SIZE(1);

    // Ensure there were no errors when invoking streamOut_out
    ASSERT_EVENTS_StreamOutError_SIZE(0);

    return Drv::SendStatus::SEND_OK;
  }


} // end namespace Drv
