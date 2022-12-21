// ======================================================================
// \title  Tester.hpp
// \author bocchino, mereweth
// \brief  BufferAccumulator test harness implementation
//
// \copyright
// Copyright 2009-2017, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Tester.hpp"
#include <FpConfig.hpp>
#include "Fw/Types/MallocAllocator.hpp"

#define INSTANCE 0
#define MAX_HISTORY_SIZE 10
#define QUEUE_DEPTH 10

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  Tester ::
    Tester(bool doAllocateQueue) :
      BufferAccumulatorGTestBase("Tester", MAX_HISTORY_SIZE),
      component("BufferAccumulator"),
      doAllocateQueue(doAllocateQueue)
  {
    this->initComponents();
    this->connectPorts();

    if (this->doAllocateQueue) {
      Fw::MallocAllocator buffAccumMallocator;
      this->component.allocateQueue(0,buffAccumMallocator,MAX_NUM_BUFFERS);
    }
  }

  Tester ::
    ~Tester()
  {
    if (this->doAllocateQueue) {
      Fw::MallocAllocator buffAccumMallocator;
      this->component.deallocateQueue(buffAccumMallocator);
    }
  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  void Tester ::
    AccumNoAllocate()
  {
    // TODO (mereweth) - make something sensible happen when no-one sets us up
  }

  // ----------------------------------------------------------------------
  // Handlers for typed from ports
  // ----------------------------------------------------------------------

  void Tester ::
    from_bufferSendOutDrain_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer& fwBuffer
    )
  {
    this->pushFromPortEntry_bufferSendOutDrain(fwBuffer);
  }

  void Tester ::
    from_bufferSendOutReturn_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer& fwBuffer
    )
  {
    this->pushFromPortEntry_bufferSendOutReturn(fwBuffer);
  }

  void Tester ::
    from_pingOut_handler(
        const NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
    this->pushFromPortEntry_pingOut(key);
  }

  // ----------------------------------------------------------------------
  // Helper methods
  // ----------------------------------------------------------------------

  void Tester ::
    connectPorts()
  {

    // bufferSendInFill
    this->connect_to_bufferSendInFill(
        0,
        this->component.get_bufferSendInFill_InputPort(0)
    );

    // bufferSendInReturn
    this->connect_to_bufferSendInReturn(
        0,
        this->component.get_bufferSendInReturn_InputPort(0)
    );

    // cmdIn
    this->connect_to_cmdIn(
        0,
        this->component.get_cmdIn_InputPort(0)
    );

    // pingIn
    this->connect_to_pingIn(
        0,
        this->component.get_pingIn_InputPort(0)
    );

    // schedIn
    this->connect_to_schedIn(
        0,
        this->component.get_schedIn_InputPort(0)
    );
    // bufferSendOutDrain
    this->component.set_bufferSendOutDrain_OutputPort(
        0,
        this->get_from_bufferSendOutDrain(0)
    );

    // bufferSendOutReturn
    this->component.set_bufferSendOutReturn_OutputPort(
        0,
        this->get_from_bufferSendOutReturn(0)
    );

    // cmdRegOut
    this->component.set_cmdRegOut_OutputPort(
        0,
        this->get_from_cmdRegOut(0)
    );

    // cmdResponseOut
    this->component.set_cmdResponseOut_OutputPort(
        0,
        this->get_from_cmdResponseOut(0)
    );

    // eventOut
    this->component.set_eventOut_OutputPort(
        0,
        this->get_from_eventOut(0)
    );

    // eventOutText
    this->component.set_eventOutText_OutputPort(
        0,
        this->get_from_eventOutText(0)
    );
    // pingOut
    this->component.set_pingOut_OutputPort(
        0,
        this->get_from_pingOut(0)
    );

    // timeCaller
    this->component.set_timeCaller_OutputPort(
        0,
        this->get_from_timeCaller(0)
    );

    // tlmOut
    this->component.set_tlmOut_OutputPort(
        0,
        this->get_from_tlmOut(0)
    );


  }

  void Tester ::
    initComponents()
  {
    this->init();
    this->component.init(
        QUEUE_DEPTH, INSTANCE
    );
  }

} // end namespace Svc
