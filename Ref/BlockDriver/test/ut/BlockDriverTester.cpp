// ======================================================================
// \title  BlockDriverTester.cpp
// \author root
// \brief  cpp file for BlockDriver component test harness implementation class
// ======================================================================

#include "BlockDriverTester.hpp"

namespace Drv {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  BlockDriverTester ::
    BlockDriverTester() :
      BlockDriverGTestBase("BlockDriverTester", BlockDriverTester::MAX_HISTORY_SIZE),
      component("BlockDriver")
  {
    this->initComponents();
    this->connectPorts();
  }

  BlockDriverTester ::
    ~BlockDriverTester()
  {

  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  void BlockDriverTester ::
    testDataLoopBack()
  {
    const U8 data[] = {1, 2, 3, 4, 5, 6, 7};
    Drv::DataBuffer dataBuffer(data, 7);

    this->clearHistory();

    // input data
    this->invoke_to_BufferIn(0, dataBuffer);

    this->component.doDispatch();

    // verify data output
    ASSERT_from_BufferOut_SIZE(1);
    ASSERT_from_BufferOut(0, dataBuffer);
  }

  void BlockDriverTester ::
    testPing()
  {
    const U32 key = 42;

    this->clearHistory();

    // call Ping port with the key
    this->invoke_to_PingIn(0, key);

    this->component.doDispatch();

    // verify Ping output
    ASSERT_from_PingOut_SIZE(1);
    ASSERT_from_PingOut(0, key);
  }

  void BlockDriverTester ::
    testIsrInvocation()
  {
    this->clearHistory();

    this->component.callIsr();

    // verify telemetry right after ISR call: it shall be empty
    // the ISR is run in the internal thread
    ASSERT_TLM_SIZE(0);
    ASSERT_TLM_BD_Cycles_SIZE(0);

    this->component.doDispatch();

    // now the telemetry shall be updated
    ASSERT_TLM_SIZE(1);
    ASSERT_TLM_BD_Cycles_SIZE(1);
    ASSERT_TLM_BD_Cycles(0, 0);
  }

  void BlockDriverTester ::
    testCycleIncrement()
  {
    this->clearHistory();

    // call ISR
    this->component.callIsr();
    this->component.doDispatch();

    // there shall be one report with 0 cycle
    ASSERT_TLM_SIZE(1);
    ASSERT_TLM_BD_Cycles_SIZE(1);
    ASSERT_TLM_BD_Cycles(0, 0);

    // call ISR once again
    this->component.callIsr();
    this->component.doDispatch();

    // there shall be one more report with 1 cycle
    ASSERT_TLM_SIZE(2);
    ASSERT_TLM_BD_Cycles_SIZE(2);
    ASSERT_TLM_BD_Cycles(1, 1);
  }

}
