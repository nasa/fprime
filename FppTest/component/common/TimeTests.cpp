#include "test/ut/Tester.hpp"
#include "Fw/Time/Time.hpp"

// ----------------------------------------------------------------------
// Time test
// ----------------------------------------------------------------------

void Tester ::
  testTime()
{
  Fw::Time zero_time(TB_NONE, 0, 0);
  Fw::Time time;
  Fw::Time result;

  result = component.getTime();
  ASSERT_EQ(result, zero_time);

  this->connectTimeGetOut();
  ASSERT_TRUE(component.isConnected_timeGetOut_OutputPort(0));

  result = component.getTime();
  ASSERT_EQ(result, time);

  this->connectSpecialPortsSerial();
  ASSERT_TRUE(component.isConnected_timeGetOut_OutputPort(0));

  result = component.getTime();
  ASSERT_EQ(result, time);
}
