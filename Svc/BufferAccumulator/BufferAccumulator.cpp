// ====================================================================== 
// \title  BufferAccumulator.cpp
// \author bocchino
// \brief  BufferAccumulator implementation
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
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

#include "ASTERIA/Components/BufferAccumulator/BufferAccumulator.hpp"
#include "Fw/Types/BasicTypes.hpp"

namespace ASTERIA {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction 
  // ----------------------------------------------------------------------

  BufferAccumulator ::
    BufferAccumulator(
        const char *const compName,
        const U32 maxNumBuffers
    ) :
      BufferAccumulatorComponentBase(compName),
      mode(BufferAccumulatorMode::DRAIN),
      bufferMemory(new Fw::Buffer[maxNumBuffers]),
      bufferArray(this->bufferMemory, maxNumBuffers),
      bufferQueue(this->bufferArray),
      send(true),
      numWarnings(0)
  {

  }

  void BufferAccumulator ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    ) 
  {
    BufferAccumulatorComponentBase::init(queueDepth, instance);
  }

  BufferAccumulator ::
    ~BufferAccumulator(void)
  {
    delete[] this->bufferMemory;
  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void BufferAccumulator ::
    bufferSendInFill_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer buffer
    )
  {
    const bool status = this->bufferQueue.enqueue(buffer);
    if (status) {
      if (this->numWarnings > 0) {
        this->log_ACTIVITY_HI_BufferAccepted();
      }
      this->numWarnings = 0;
    }
    else {
      if (this->numWarnings == 0) {
        this->log_WARNING_HI_QueueFull();
      }
      ++numWarnings;
    }
    if (this->send) {
      this->sendStoredBuffer();
    }
  }

  void BufferAccumulator ::
    bufferSendInReturn_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer buffer
    )
  {
    this->bufferSendOutReturn_out(0, buffer);
    this->send = true;
    this->sendStoredBuffer();
  }

  void BufferAccumulator ::
    pingIn_handler(
        const NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
    this->pingOut_out(0, key);
  }

  // ----------------------------------------------------------------------
  // Command handler implementations 
  // ----------------------------------------------------------------------

  void BufferAccumulator ::
    SetMode_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        BufferAccumulatorMode mode
    )
  {
    this->mode = mode;
    if (mode == BufferAccumulatorMode::DRAIN) {
      this->send = true;
      this->sendStoredBuffer();
    }
    else {
      this->send = false;
    }
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
  }

  // ----------------------------------------------------------------------
  // Private helper methods 
  // ----------------------------------------------------------------------

  void BufferAccumulator ::
    sendStoredBuffer(void)
  {
    FW_ASSERT(this->send);
    Fw::Buffer buffer;
    const bool status = this->bufferQueue.dequeue(buffer);
    if (status) {
      this->bufferSendOutDrain_out(0, buffer);
      this->send = false;
    }
  }

}
