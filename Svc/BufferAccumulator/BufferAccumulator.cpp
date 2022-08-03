// ======================================================================
// \title  BufferAccumulator.cpp
// \author bocchino
// \brief  BufferAccumulator implementation
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Svc/BufferAccumulator/BufferAccumulator.hpp"
#include <FpConfig.hpp>

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  BufferAccumulator ::
      BufferAccumulator(const char *const compName) :
        BufferAccumulatorComponentBase(compName),
        mode(DRAIN),
        bufferMemory(nullptr),
        bufferQueue(),
        send(true),
        numWarnings(0),
        allocatorId(0)
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
    ~BufferAccumulator()
  {
  }

  // ----------------------------------------------------------------------
  // Public methods
  // ----------------------------------------------------------------------

  void BufferAccumulator ::
    allocateQueue(
      NATIVE_INT_TYPE identifier,
      Fw::MemAllocator& allocator,
      NATIVE_UINT_TYPE maxNumBuffers //!< The maximum number of buffers
    )
  {
      this->allocatorId = identifier;
      bool recoverable; // don't need to recover
      NATIVE_UINT_TYPE actualSize = sizeof(Fw::Buffer) * maxNumBuffers;
      this->bufferMemory =  static_cast<Fw::Buffer*>(
          allocator.allocate(identifier, actualSize, recoverable));
      NATIVE_UINT_TYPE actualBuffers = actualSize/sizeof(Fw::Buffer);

      bufferQueue.init(this->bufferMemory, actualBuffers);
  }

  void BufferAccumulator ::
    deallocateQueue(Fw::MemAllocator& allocator)
  {
      allocator.deallocate(this->allocatorId, (void*)this->bufferMemory);
  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void BufferAccumulator ::
    bufferSendInFill_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer& buffer
    )
  {
    const bool status = this->bufferQueue.enqueue(buffer);
    if (status) {
      if (this->numWarnings > 0) {
        this->log_ACTIVITY_HI_BA_BufferAccepted();
      }
      this->numWarnings = 0;
    }
    else {
      if (this->numWarnings == 0) {
        this->log_WARNING_HI_BA_QueueFull();
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
        Fw::Buffer& buffer
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

  void BufferAccumulator ::
    schedIn_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    // TODO
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void BufferAccumulator ::
    BA_SetMode_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        OpState mode
    )
  {
    this->mode = mode;
    if (mode == DRAIN) {
      this->send = true;
      this->sendStoredBuffer();
    }
    else {
      this->send = false;
    }
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  // ----------------------------------------------------------------------
  // Private helper methods
  // ----------------------------------------------------------------------

  void BufferAccumulator ::
    sendStoredBuffer()
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
