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

#include <sys/time.h>

#include "Fw/Types/BasicTypes.hpp"


namespace Svc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

BufferAccumulator ::
    BufferAccumulator(const char* const compName)
    : BufferAccumulatorComponentBase(compName),  //!< The component name
      mode(BufferAccumulator_OpState::ACCUMULATE),
      bufferMemory(nullptr),
      bufferQueue(),
      send(false),
      waitForBuffer(false),
      numWarnings(0u),
      numDrained(0u),
      numToDrain(0u),
      opCode(),
      cmdSeq(0u),
      allocatorId(0) {
}

void BufferAccumulator ::init(const NATIVE_INT_TYPE queueDepth,
                              const NATIVE_INT_TYPE instance) {
  BufferAccumulatorComponentBase::init(queueDepth, instance);
}

BufferAccumulator ::~BufferAccumulator() {}

// ----------------------------------------------------------------------
// Public methods
// ----------------------------------------------------------------------

void BufferAccumulator ::allocateQueue(
    NATIVE_INT_TYPE identifier, Fw::MemAllocator& allocator,
    NATIVE_UINT_TYPE maxNumBuffers  //!< The maximum number of buffers
) {

  this->allocatorId = identifier;
  NATIVE_UINT_TYPE memSize = sizeof(Fw::Buffer) * maxNumBuffers;
  bool recoverable = false;
  this->bufferMemory = static_cast<Fw::Buffer*>(
      allocator.allocate(identifier, memSize, recoverable));
  //TODO: Fail gracefully here
  bufferQueue.init(this->bufferMemory, maxNumBuffers);
}

void BufferAccumulator ::deallocateQueue(Fw::MemAllocator& allocator) {
  allocator.deallocate(this->allocatorId, this->bufferMemory);
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void BufferAccumulator ::bufferSendInFill_handler(const NATIVE_INT_TYPE portNum,
                                                  Fw::Buffer& buffer) {

  const bool status = this->bufferQueue.enqueue(buffer);
  if (status) {
    if (this->numWarnings > 0) {
      this->log_ACTIVITY_HI_BA_BufferAccepted();
    }
    this->numWarnings = 0;
  } else {
    if (this->numWarnings == 0) {
      this->log_WARNING_HI_BA_QueueFull();
    }
    numWarnings++;
  }
  if (this->send) {
    this->sendStoredBuffer();
  }

  this->tlmWrite_BA_NumQueuedBuffers(this->bufferQueue.getSize());
}

void BufferAccumulator ::bufferSendInReturn_handler(
    const NATIVE_INT_TYPE portNum, Fw::Buffer& buffer) {

  this->bufferSendOutReturn_out(0, buffer);
  this->waitForBuffer = false;
  if ((this->mode == BufferAccumulator_OpState::DRAIN) ||  // we are draining ALL buffers
      (this->numDrained < this->numToDrain)) {  // OR we aren't done draining some buffers
                                                // in a partial drain
    this->send = true;
    this->sendStoredBuffer();
  }
}

void BufferAccumulator ::pingIn_handler(const NATIVE_INT_TYPE portNum,
                                        U32 key) {
  this->pingOut_out(0, key);
}

// ----------------------------------------------------------------------
// Command handler implementations
// ----------------------------------------------------------------------

void BufferAccumulator ::BA_SetMode_cmdHandler(const FwOpcodeType opCode,
                                               const U32 cmdSeq,
                                               BufferAccumulator_OpState mode) {

  // cancel an in-progress partial drain
  if (this->numToDrain > 0) {
    // reset counters for partial buffer drain
    this->numToDrain = 0;
    this->numDrained = 0;
    // respond to the original command
    this->cmdResponse_out(this->opCode, this->cmdSeq, Fw::CmdResponse::OK);
  }

  this->mode = mode;
  if (mode == BufferAccumulator_OpState::DRAIN) {
    if (!this->waitForBuffer) {
      this->send = true;
      this->sendStoredBuffer();
    }
  } else {
    this->send = false;
  }
  this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void BufferAccumulator ::BA_DrainBuffers_cmdHandler(
    const FwOpcodeType opCode, const U32 cmdSeq, U32 numToDrain,
    BufferAccumulator_BlockMode blockMode) {

  if (this->numDrained < this->numToDrain) {
    this->log_WARNING_HI_BA_StillDraining(this->numDrained, this->numToDrain);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::BUSY);
    return;
  }

  if (this->mode == BufferAccumulator_OpState::DRAIN) {
    this->log_WARNING_HI_BA_AlreadyDraining();
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::VALIDATION_ERROR);
    return;
  }

  if (numToDrain == 0) {
    this->log_ACTIVITY_HI_BA_PartialDrainDone(0);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    return;
  }

  this->opCode = opCode;
  this->cmdSeq = cmdSeq;
  this->numDrained = 0;
  this->numToDrain = numToDrain;

  if (blockMode == BufferAccumulator_BlockMode::NOBLOCK) {
    U32 numBuffers = this->bufferQueue.getSize();

    if (numBuffers < numToDrain) {
      this->numToDrain = numBuffers;
      this->log_WARNING_LO_BA_NonBlockDrain(this->numToDrain, numToDrain);
    }

    /* OK if there were 0 buffers queued, and we
     * end up setting numToDrain to 0
     */
    if (0 == this->numToDrain) {
      this->log_ACTIVITY_HI_BA_PartialDrainDone(0);
      this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
      return;
    }
  }

  // We are still waiting for a buffer from last time
  if (!this->waitForBuffer) {
    this->send = true;
    this->sendStoredBuffer();  // kick off the draining;
  }
}

// ----------------------------------------------------------------------
// Private helper methods
// ----------------------------------------------------------------------

void BufferAccumulator ::sendStoredBuffer() {

  FW_ASSERT(this->send);
  Fw::Buffer buffer;
  if ((this->numToDrain == 0) ||  // we are draining ALL buffers
      (this->numDrained < this->numToDrain)) {  // OR we aren't done draining some buffers in a
                                                // partial drain
    const bool status = this->bufferQueue.dequeue(buffer);
    if (status) {  // a buffer was dequeued
      this->numDrained++;
      this->bufferSendOutDrain_out(0, buffer);
      this->waitForBuffer = true;
      this->send = false;
    } else if (this->numToDrain > 0) {
      this->log_WARNING_HI_BA_DrainStalled(this->numDrained, this->numToDrain);
    }
  }

  /* This used to be "else if", but then you wait for all
   * drained buffers in a partial drain to be RETURNED before returning OK.
   * Correct thing is to return OK once they are SENT
   */
  if ((this->numToDrain > 0) &&  // we are doing a partial drain
      (this->numDrained == this->numToDrain)) {  // AND we just finished draining
                                                 //
    this->log_ACTIVITY_HI_BA_PartialDrainDone(this->numDrained);
    // reset counters for partial buffer drain
    this->numToDrain = 0;
    this->numDrained = 0;
    this->send = false;
    this->cmdResponse_out(this->opCode, this->cmdSeq, Fw::CmdResponse::OK);
  }

  this->tlmWrite_BA_NumQueuedBuffers(this->bufferQueue.getSize());
}

}  // namespace Svc
