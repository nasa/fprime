// ====================================================================== 
// \title  BufferLogger.cpp
// \author bocchino, dinkel
// \brief  ASTERIA BufferLogger implementation
//
// \copyright
// Copyright (C) 2015-2017 California Institute of Technology.
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

#include "ASTERIA/Components/BufferLogger/BufferLogger.hpp"

namespace ASTERIA {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction 
  // ----------------------------------------------------------------------

  BufferLogger ::
    BufferLogger(
        const char *const compName,
        const char *const stateFilePath,
        const char *const logFilePrefix,
        const char *const logFileSuffix,
        const U32 maxFileSize,
        const U8 sizeOfSize
    ) :
      BufferLoggerComponentBase(compName), 
      state(*this, stateFilePath),
      file(*this, logFilePrefix, logFileSuffix, maxFileSize, sizeOfSize)
  {

  }

  void BufferLogger :: 
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    )
  {
    BufferLoggerComponentBase::init(queueDepth, instance);
  }

  void BufferLogger ::
    setup(void)
  {
    this->state.load();
  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void BufferLogger ::
    bufferSendIn_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer fwBuffer
    )
  {
    const OnOff::t state = this->state.get();
    if (state == OnOff::ON) {
      const U8 *const addr = reinterpret_cast<U8*>(fwBuffer.getdata());
      const U32 size = fwBuffer.getsize();
      this->file.logBuffer(addr, size);
    }
    this->bufferSendOut_out(0, fwBuffer);
  }

  void BufferLogger ::
    comIn_handler(
        NATIVE_INT_TYPE portNum,
        Fw::ComBuffer &data,
        U32 context
    )
  {
    const OnOff::t state = this->state.get();
    if (state == OnOff::ON) {
      const U8 *const addr = data.getBuffAddr();
      const U32 size = data.getBuffLength();
      this->file.logBuffer(addr, size);
    }
  }

  void BufferLogger ::
    pingIn_handler(NATIVE_INT_TYPE portNum, U32 key)
  {
    this->pingOut_out(0, key);
  }

  // ----------------------------------------------------------------------
  // Command handler implementations 
  // ----------------------------------------------------------------------

  void BufferLogger :: 
    CloseFile_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    this->file.closeAndEmitEvent();
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
  }

  void BufferLogger ::
    SetSaveState_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        OnOff state
    )
  {
    if (state.e == OnOff::OFF) {
      this->file.closeAndEmitEvent();
    }
    this->state.set(state.e);
    const NonVolatileU8::Status::t status = this->state.save();
    if (status == NonVolatileU8::Status::SUCCESS) {
      this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
    }
    else {
      this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
    }
  }

  void BufferLogger ::
  SetVolatileState_cmdHandler(
    const FwOpcodeType opCode,
    const U32 cmdSeq,
    OnOff state
  )
  {
    if (state.e == OnOff::OFF) {
      this->file.closeAndEmitEvent();
    }
    this->state.set(state.e);
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
  }

  void BufferLogger ::
  SaveState_cmdHandler(
    const FwOpcodeType opCode,
    const U32 cmdSeq
  )
  {
    const NonVolatileU8::Status::t status = this->state.save();
    if (status == NonVolatileU8::Status::SUCCESS) {
      this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
    }
    else {
      this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
    }
  }
        
  void BufferLogger ::
  LoadState_cmdHandler(
    const FwOpcodeType opCode,
    const U32 cmdSeq
  )
  {
    const NonVolatileU8::Status::t status = this->state.load();
    if (status == NonVolatileU8::Status::SUCCESS) {
      this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
    }
    else {
      this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
    }
  }

  void BufferLogger ::
  Flush_cmdHandler(
    const FwOpcodeType opCode,
    const U32 cmdSeq
  )
  {
    const bool status = this->file.flush();
    if(status)
    {
      this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
    }
    else
    {
      this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
    }
  }

};
