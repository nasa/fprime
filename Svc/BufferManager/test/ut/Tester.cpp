// ====================================================================== 
// \title  BufferManager/test/ut/Tester.hpp
// \author bocchino
// \brief  cpp file for BufferManager test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// 
// This software may be subject to U.S. export control laws and
// regulations.  By accepting this document, the user agrees to comply
// with all U.S. export laws and regulations.  User has the
// responsibility to obtain export licenses, or other export authority
// as may be required before exporting such information to foreign
// countries or providing access to foreign persons.
// ====================================================================== 


#include <stdio.h>
#include <strings.h>

#include "Tester.hpp"
#include "Fw/Cmd/CmdPacket.hpp"

#define MAX_HISTORY_SIZE 10
#define STORE_SIZE 1024
#define ALLOCATION_QUEUE_SIZE 10
#define INSTANCE 0

namespace Svc {

  Tester ::
    Tester(void) : 
      BufferManagerGTestBase("Tester", MAX_HISTORY_SIZE),
      bufferManager("BufferManager", STORE_SIZE, ALLOCATION_QUEUE_SIZE)
  {
    this->connectPorts();
    this->initComponents();
  }

  Tester ::
    ~Tester(void) 
  {
    
  }

  // ----------------------------------------------------------------------
  // Helper methods 
  // ----------------------------------------------------------------------

  void Tester ::
    connectPorts(void) 
  {

    // timeCaller
    this->bufferManager.set_timeCaller_OutputPort(
        0, 
        this->get_from_timeCaller(0)
    );

    // bufferSendIn
    this->connect_to_bufferSendIn(
        0,
        this->bufferManager.get_bufferSendIn_InputPort(0)
    );

    // tlmOut
    this->bufferManager.set_tlmOut_OutputPort(
        0, 
        this->get_from_tlmOut(0)
    );

    // eventOut
    this->bufferManager.set_eventOut_OutputPort(
        0, 
        this->get_from_eventOut(0)
    );

    // bufferGetCallee
    this->connect_to_bufferGetCallee(
        0,
        this->bufferManager.get_bufferGetCallee_InputPort(0)
    );

  }

  void Tester ::
    initComponents(void) 
  {
    this->init();
    this->bufferManager.init(INSTANCE);
  }

  // ----------------------------------------------------------------------
  // Tests 
  // ----------------------------------------------------------------------

  void Tester ::
    allocateAndFreeOne(void) 
  {

    Fw::Buffer b = this->invoke_to_bufferGetCallee(0, 10);

    ASSERT_EVENTS_SIZE(0);

    ASSERT_TLM_SIZE(2);
    ASSERT_TLM_BufferManager_NumAllocatedBuffers_SIZE(1);
    ASSERT_TLM_BufferManager_NumAllocatedBuffers(0, 1);
    ASSERT_TLM_BufferManager_AllocatedSize_SIZE(1);
    ASSERT_TLM_BufferManager_AllocatedSize(0, 10);

    ASSERT_EQ(
        static_cast<U32>(this->bufferManager.getInstance()),
        b.getmanagerID()
    );
    ASSERT_EQ(
        static_cast<U32>(0),
        b.getbufferID()
    );
    // Make sure the allocated memory is good for writing
    U8 *const data = reinterpret_cast<U8*>(b.getdata());
    bzero(data, 10);
    ASSERT_EQ(
        static_cast<U32>(10),
        b.getsize()
    );

    this->clearTlm();
    this->invoke_to_bufferSendIn(0, b);

    ASSERT_EVENTS_SIZE(0);

    ASSERT_TLM_SIZE(2);
    ASSERT_TLM_BufferManager_NumAllocatedBuffers_SIZE(1);
    ASSERT_TLM_BufferManager_NumAllocatedBuffers(0, 0);
    ASSERT_TLM_BufferManager_AllocatedSize_SIZE(1);
    ASSERT_TLM_BufferManager_AllocatedSize(0, 0);
    
  }

  void Tester ::
    allocationQueueEmpty(void) 
  {

    Fw::Buffer b;
    b.setmanagerID(this->getInstance());
    this->invoke_to_bufferSendIn(0, b);

    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_BufferManager_AllocationQueueEmpty_SIZE(1);

    ASSERT_TLM_SIZE(0);

  }

  void Tester ::
    allocationQueueFull(void) 
  {

    for (U32 i = 0; i < ALLOCATION_QUEUE_SIZE; ++i) {
      (void) this->invoke_to_bufferGetCallee(0, 10);
      ASSERT_EVENTS_SIZE(0);
      ASSERT_TLM_SIZE(2 * (i+1));
      ASSERT_TLM_BufferManager_NumAllocatedBuffers_SIZE(i+1);
      ASSERT_TLM_BufferManager_NumAllocatedBuffers(i, i+1);
      ASSERT_TLM_BufferManager_AllocatedSize_SIZE(i+1);
      ASSERT_TLM_BufferManager_AllocatedSize(i, 10 * (i+1));
    }

    Fw::Buffer b = this->invoke_to_bufferGetCallee(0, 10);
    // Assert that buffer is invalid
    ASSERT_EQ(static_cast<U64>(0), b.getdata());
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_BufferManager_AllocationQueueFull_SIZE(1);
    ASSERT_TLM_SIZE(2 * ALLOCATION_QUEUE_SIZE);

  }

  void Tester ::
    idMismatch(void) 
  {

    Fw::Buffer b = this->invoke_to_bufferGetCallee(0, 10);

    ASSERT_EVENTS_SIZE(0);

    ASSERT_TLM_SIZE(2);
    ASSERT_TLM_BufferManager_NumAllocatedBuffers_SIZE(1);
    ASSERT_TLM_BufferManager_NumAllocatedBuffers(0, 1);
    ASSERT_TLM_BufferManager_AllocatedSize_SIZE(1);
    ASSERT_TLM_BufferManager_AllocatedSize(0, 10);

    b.setbufferID(b.getbufferID() + 1);

    this->clearTlm();
    this->invoke_to_bufferSendIn(0, b);

    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_BufferManager_IDMismatch_SIZE(1);

    ASSERT_TLM_SIZE(0);

  }

  void Tester ::
    storeSizeExceeded(void) 
  {

    Fw::Buffer b = this->invoke_to_bufferGetCallee(0, STORE_SIZE + 1);
    // Assert that buffer is invalid
    ASSERT_EQ(static_cast<U64>(0), b.getdata());
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_BufferManager_StoreSizeExceeded_SIZE(1);
    ASSERT_TLM_SIZE(0);

  }

};
