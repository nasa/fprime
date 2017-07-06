// ----------------------------------------------------------------------
// LinuxTime/test/ut/Tester.cpp
// ----------------------------------------------------------------------

#include <stdio.h>
#include <strings.h>

#include "Tester.hpp"

#define INSTANCE 0

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction and destruction 
  // ----------------------------------------------------------------------

  Tester ::
    Tester(const char *const compName) : 
      TimeGTestBase(compName, 0),
      linuxTime("LinuxTime")
  {
    this->init();
    this->linuxTime.init(INSTANCE);
    this->connect_to_timeGetPort(
        0, 
        this->linuxTime.get_timeGetPort_InputPort(0)
    );
  }

  Tester ::
    ~Tester(void) 
  {
    
  }

  // ----------------------------------------------------------------------
  // Tests 
  // ----------------------------------------------------------------------

  void Tester ::
    getTime(void) 
  {
    Fw::Time time;
    this->invoke_to_timeGetPort(0,time);
    //printf("seconds: %d\n", time.getSeconds());
    //printf("useconds: %d\n", time.getUSeconds());
    ASSERT_GT(time.getSeconds(), 0U);
    ASSERT_GE(time.getUSeconds(), 0U);
    ASSERT_LE(time.getUSeconds(), 999999U);
  }

};
