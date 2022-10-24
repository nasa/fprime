// ======================================================================
// \title  LinuxI2cDriver.hpp
// \author tcanham
// \brief  cpp file for LinuxI2cDriver test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Tester.hpp"

#define INSTANCE 0
#define MAX_HISTORY_SIZE 10

namespace Drv {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  Tester ::
    Tester() :
      LinuxI2cDriverGTestBase("Tester", MAX_HISTORY_SIZE),
      component("LinuxI2cDriver")
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
    sendData(U32 addr, U8* data, NATIVE_INT_TYPE size)
  {
      Fw::Buffer dataBuff;
      dataBuff.setdata(static_cast<POINTER_CAST>(data));
      dataBuff.setsize(size);
      this->invoke_to_write(0,addr,dataBuff);
  }

  void Tester::open(const char* device) {
	  this->component.open(device);
  }


  // ----------------------------------------------------------------------
  // Helper methods
  // ----------------------------------------------------------------------

  void Tester ::
    connectPorts()
  {

    // write
    this->connect_to_write(
        0,
        this->component.get_write_InputPort(0)
    );




  }

  void Tester ::
    initComponents()
  {
    this->init();
    this->component.init(
        INSTANCE
    );
  }

} // end namespace Drv
