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

#include "Svc/BufferAccumulator/BufferAccumulator.hpp"
#include "Fw/Types/BasicTypes.hpp"

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  BufferAccumulator ::
#if FW_OBJECT_NAMES == 1
      BufferAccumulator(const char *const compName) :
          BufferAccumulatorComponentBase(compName), //!< The component name
#else
      BufferAccumulator() : BufferAccumulatorComponentBase(),
#endif
      mode(DRAIN),
      bufferMemory(NULL),
      bufferQueue(),
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
  }

  // ----------------------------------------------------------------------
  // Public methods
  // ----------------------------------------------------------------------

  void BufferAccumulator ::
    allocateQueue(
      NATIVE_INT_TYPE identifier,
      Fw::MemAllocator& allocator,
      NATIVE_UINT_TYPE bytesPerBuffer, //!< Storage for each Fw::Buffer
      U32 maxNumBuffers //!< The maximum number of buffers
    )
  {
      this->allocatorId = identifier;
      this->bufferMemory =  static_cast<Fw::Buffer*>(
          allocator.allocate(identifier, bytesPerBuffer * maxNumBuffers));
      bufferQueue.init(this->bufferMemory, maxNumBuffers);
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
        Fw::Buffer buffer
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
