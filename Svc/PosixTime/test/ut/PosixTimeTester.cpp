// ----------------------------------------------------------------------
// PosixTime/test/ut/Tester.cpp
// ----------------------------------------------------------------------

#include <cstdio>
#include <strings.h>

#include "PosixTimeTester.hpp"

#define INSTANCE 0

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  PosixTimeTester ::
    PosixTimeTester(const char *const compName) :
      PosixTimeGTestBase(compName, 0),
      component("PosixTime")
  {
    this->init();
    this->component.init(INSTANCE);
    this->connect_to_timeGetPort(
        0,
        this->component.get_timeGetPort_InputPort(0)
    );
  }

  PosixTimeTester ::
    ~PosixTimeTester()
  {

  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  void PosixTimeTester ::
    getTime()
  {
    Fw::Time time;
    this->invoke_to_timeGetPort(0,time);
    ASSERT_GT(time.getSeconds(), 0U);
    ASSERT_GE(time.getUSeconds(), 0U);
    ASSERT_LE(time.getUSeconds(), 999999U);
  }

};
