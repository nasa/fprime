#include "test/ut/Tester.hpp"

// ----------------------------------------------------------------------
// Time test
// ----------------------------------------------------------------------

void Tester ::
  testTime()
{
  component.getTime();
  this->connectTimeGetIn();
  component.getTime();
  this->connectSpecialPortsSerial();
  component.getTime();
}
