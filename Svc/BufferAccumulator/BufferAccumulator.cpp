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
      m_mode(BufferAccumulator_OpState::ACCUMULATE),
      m_bufferMemory(nullptr),
      m_bufferQueue(),
      m_send(false),
      m_waitForBuffer(false),
      m_numWarnings(0u),
      m_numDrained(0u),
      m_numToDrain(0u),
      m_opCode(),
      m_cmdSeq(0u),
      m_allocatorId(0) {
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

  this->m_allocatorId = identifier;
  NATIVE_UINT_TYPE memSize = sizeof(Fw::Buffer) * maxNumBuffers;
  bool recoverable = false;
  this->m_bufferMemory = static_cast<Fw::Buffer*>(
      allocator.allocate(identifier, memSize, recoverable));
  //TODO: Fail gracefully here
  m_bufferQueue.init(this->m_bufferMemory, maxNumBuffers);
}

void BufferAccumulator ::deallocateQueue(Fw::MemAllocator& allocator) {
  allocator.deallocate(this->m_allocatorId, this->m_bufferMemory);
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void BufferAccumulator ::bufferSendInFill_handler(const NATIVE_INT_TYPE portNum,
                                                  Fw::Buffer& buffer) {

  const bool status = this->m_bufferQueue.enqueue(buffer);
  if (status) {
    if (this->m_numWarnings > 0) {
      this->log_ACTIVITY_HI_BA_BufferAccepted();
    }
    this->m_numWarnings = 0;
  } else {
    if (this->m_numWarnings == 0) {
      this->log_WARNING_HI_BA_QueueFull();
    }
    m_numWarnings++;
  }
  if (this->m_send) {
    this->sendStoredBuffer();
  }

  this->tlmWrite_BA_NumQueuedBuffers(this->m_bufferQueue.getSize());
}

void BufferAccumulator ::bufferSendInReturn_handler(
    const NATIVE_INT_TYPE portNum, Fw::Buffer& buffer) {

  this->bufferSendOutReturn_out(0, buffer);
  this->m_waitForBuffer = false;
  if ((this->m_mode == BufferAccumulator_OpState::DRAIN) ||  // we are draining ALL buffers
      (this->m_numDrained < this->m_numToDrain)) {  // OR we aren't done draining some buffers
                                                // in a partial drain
    this->m_send = true;
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
  if (this->m_numToDrain > 0) {
    // reset counters for partial buffer drain
    this->m_numToDrain = 0;
    this->m_numDrained = 0;
    // respond to the original command
    this->cmdResponse_out(this->m_opCode, this->m_cmdSeq, Fw::CmdResponse::OK);
  }

  this->m_mode = mode;
  if (mode == BufferAccumulator_OpState::DRAIN) {
    if (!this->m_waitForBuffer) {
      this->m_send = true;
      this->sendStoredBuffer();
    }
  } else {
    this->m_send = false;
  }
  this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void BufferAccumulator ::BA_DrainBuffers_cmdHandler(
    const FwOpcodeType opCode, const U32 cmdSeq, U32 numToDrain,
    BufferAccumulator_BlockMode blockMode) {

  if (this->m_numDrained < this->m_numToDrain) {
    this->log_WARNING_HI_BA_StillDraining(this->m_numDrained, this->m_numToDrain);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::BUSY);
    return;
  }

  if (this->m_mode == BufferAccumulator_OpState::DRAIN) {
    this->log_WARNING_HI_BA_AlreadyDraining();
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::VALIDATION_ERROR);
    return;
  }

  if (numToDrain == 0) {
    this->log_ACTIVITY_HI_BA_PartialDrainDone(0);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    return;
  }

  this->m_opCode = opCode;
  this->m_cmdSeq = cmdSeq;
  this->m_numDrained = 0;
  this->m_numToDrain = numToDrain;

  if (blockMode == BufferAccumulator_BlockMode::NOBLOCK) {
    U32 numBuffers = this->m_bufferQueue.getSize();

    if (numBuffers < numToDrain) {
      this->m_numToDrain = numBuffers;
      this->log_WARNING_LO_BA_NonBlockDrain(this->m_numToDrain, numToDrain);
    }

    /* OK if there were 0 buffers queued, and we
     * end up setting numToDrain to 0
     */
    if (0 == this->m_numToDrain) {
      this->log_ACTIVITY_HI_BA_PartialDrainDone(0);
      this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
      return;
    }
  }

  // We are still waiting for a buffer from last time
  if (!this->m_waitForBuffer) {
    this->m_send = true;
    this->sendStoredBuffer();  // kick off the draining;
  }
}

// ----------------------------------------------------------------------
// Private helper methods
// ----------------------------------------------------------------------

void BufferAccumulator ::sendStoredBuffer() {

  FW_ASSERT(this->m_send);
  Fw::Buffer buffer;
  if ((this->m_numToDrain == 0) ||  // we are draining ALL buffers
      (this->m_numDrained < this->m_numToDrain)) {  // OR we aren't done draining some buffers in a
                                                // partial drain
    const bool status = this->m_bufferQueue.dequeue(buffer);
    if (status) {  // a buffer was dequeued
      this->m_numDrained++;
      this->bufferSendOutDrain_out(0, buffer);
      this->m_waitForBuffer = true;
      this->m_send = false;
    } else if (this->m_numToDrain > 0) {
      this->log_WARNING_HI_BA_DrainStalled(this->m_numDrained, this->m_numToDrain);
    }
  }

  /* This used to be "else if", but then you wait for all
   * drained buffers in a partial drain to be RETURNED before returning OK.
   * Correct thing is to return OK once they are SENT
   */
  if ((this->m_numToDrain > 0) &&  // we are doing a partial drain
      (this->m_numDrained == this->m_numToDrain)) {  // AND we just finished draining
                                                 //
    this->log_ACTIVITY_HI_BA_PartialDrainDone(this->m_numDrained);
    // reset counters for partial buffer drain
    this->m_numToDrain = 0;
    this->m_numDrained = 0;
    this->m_send = false;
    this->cmdResponse_out(this->m_opCode, this->m_cmdSeq, Fw::CmdResponse::OK);
  }

  this->tlmWrite_BA_NumQueuedBuffers(this->m_bufferQueue.getSize());
}

}  // namespace Svc
