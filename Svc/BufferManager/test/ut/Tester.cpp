// ====================================================================== 
// \title  BufferManager.hpp
// \author mstarch
// \brief  cpp file for BufferManager test harness implementation class
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

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction and destruction 
  // ----------------------------------------------------------------------

  Tester ::
    Tester(void) : 
#if FW_OBJECT_NAMES == 1
      BufferManagerGTestBase("Tester", MAX_HISTORY_SIZE),
      component("BufferManager", 12, 3)
#else
      BufferManagerGTestBase(MAX_HISTORY_SIZE),
      component()
#endif
  {
    this->initComponents();
    this->connectPorts();
  }

  Tester ::
    ~Tester(void) 
  {
    
  }

  // ----------------------------------------------------------------------
  // Tests 
  // ----------------------------------------------------------------------

  void Tester ::
    three_buffer_problem(void) 
  {
      //Alocate 2 buffers
      Fw::Buffer buffer1 = this->invoke_to_bufferGetCallee(0, 4);
      Fw::Buffer buffer2 = this->invoke_to_bufferGetCallee(0, 4);
      //Fill the 2nd buffer
      *((U32*)buffer2.getdata()) = 0xDEADBEEF;
      //Return the 1st buffer
      this->invoke_to_bufferSendIn(0, buffer1);
      //Allocate and fill 3rd buffer
      Fw::Buffer buffer3 = this->invoke_to_bufferGetCallee(0, 4);
      *((U32*)buffer3.getdata()) = 0x0;
      //Check that no warnings have happened
      //Check that buffer 2's data is intact. It Fails here.
      ASSERT_EQ(0xDEADBEEF,*((U32*)buffer2.getdata()));
  }

  // ----------------------------------------------------------------------
  // Helper methods 
  // ----------------------------------------------------------------------

  void Tester ::
    connectPorts(void) 
  {

    // bufferSendIn
    this->connect_to_bufferSendIn(
        0,
        this->component.get_bufferSendIn_InputPort(0)
    );

    // bufferGetCallee
    this->connect_to_bufferGetCallee(
        0,
        this->component.get_bufferGetCallee_InputPort(0)
    );

    // timeCaller
    this->component.set_timeCaller_OutputPort(
        0, 
        this->get_from_timeCaller(0)
    );

    // eventOut
    this->component.set_eventOut_OutputPort(
        0, 
        this->get_from_eventOut(0)
    );

    // textEventOut
    this->component.set_textEventOut_OutputPort(
        0, 
        this->get_from_textEventOut(0)
    );

    // tlmOut
    this->component.set_tlmOut_OutputPort(
        0, 
        this->get_from_tlmOut(0)
    );

  }

  void Tester ::
    initComponents(void) 
  {
    this->init();
    this->component.init(
        INSTANCE
    );
  }

} // end namespace Svc
