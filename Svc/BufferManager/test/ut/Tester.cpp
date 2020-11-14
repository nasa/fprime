// ====================================================================== 
// \title  BufferManager.hpp
// \author tcanham
// \brief  cpp file for BufferManager test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// 
// ====================================================================== 

#include "Tester.hpp"
#include <Fw/Types/MallocAllocator.hpp>

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
      component("BufferManager")
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
    testSetup(void) 
  {

      BufferManagerComponentImpl::BufferBins bins;
      memset(&bins,0,sizeof(bins));
      bins.bins[0].bufferSize = 100;
      bins.bins[0].numBuffers = 2;
      bins.bins[5].bufferSize = 12;
      bins.bins[5].numBuffers = 4;
      bins.bins[9].bufferSize = 10;
      bins.bins[9].numBuffers = 3;

      Fw::MallocAllocator alloc;

      this->component.setup(10,0,alloc,bins);
  }

  void Tester::oneBufferSize(void) {

      BufferManagerComponentImpl::BufferBins bins;
      memset(&bins,0,sizeof(bins));
      bins.bins[0].bufferSize = 100;
      bins.bins[0].numBuffers = 2;

      Fw::MallocAllocator alloc;

      this->component.setup(10,0,alloc,bins);

      // Get a buffer
      Fw::Buffer testBuff = this->invoke_to_bufferGetCallee(0,25);
      // return the buffer
      this->invoke_to_bufferSendIn(0,testBuff);

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
