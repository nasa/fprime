// ======================================================================
// \title  BufferLogger.cpp
// \author bocchino, dinkel, mereweth
// \brief  Svc BufferLogger implementation
//
// \copyright
// Copyright (C) 2015-2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Svc/BufferLogger/BufferLogger.hpp"

namespace Svc {

  typedef BufferLogger_LogState LogState;
  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  BufferLogger ::
      BufferLogger(const char *const compName) :
          BufferLoggerComponentBase(compName),
          m_state(LogState::LOGGING_ON),
          m_file(*this)
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

  // ----------------------------------------------------------------------
  // Public methods
  // ----------------------------------------------------------------------

  //TODO(mereweth) - only allow calling this once?
  void BufferLogger ::
    initLog(
        const char *const logFilePrefix,
        const char *const logFileSuffix,
        const U32 maxFileSize,
        const U8 sizeOfSize
    )
  {
      m_file.init(logFilePrefix, logFileSuffix, maxFileSize, sizeOfSize);
  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void BufferLogger ::
    bufferSendIn_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer& fwBuffer
    )
  {
    if (m_state == LogState::LOGGING_ON) {
      const U8 *const addr = fwBuffer.getData();
      const U32 size = fwBuffer.getSize();
      m_file.logBuffer(addr, size);
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
    if (m_state == LogState::LOGGING_ON) {
      const U8 *const addr = data.getBuffAddr();
      const U32 size = data.getBuffLength();
      m_file.logBuffer(addr, size);
    }
  }

  void BufferLogger ::
    pingIn_handler(NATIVE_INT_TYPE portNum, U32 key)
  {
    this->pingOut_out(0, key);
  }

  void BufferLogger ::
    schedIn_handler(
        const NATIVE_INT_TYPE portNum,
        U32 context
    )
  {
    // TODO
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  // TODO(mereweth) - should this command only set the base name?
  void BufferLogger ::
    BL_OpenFile_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CmdStringArg& file
    )
  {
    m_file.setBaseName(file);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  void BufferLogger ::
    BL_CloseFile_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    m_file.closeAndEmitEvent();
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  void BufferLogger ::
    BL_SetLogging_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        LogState state
  )
  {
    m_state = state;
    if (state == LogState::LOGGING_OFF) {
      m_file.closeAndEmitEvent();
    }
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  void BufferLogger ::
    BL_FlushFile_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    const bool status = m_file.flush();
    if(status)
    {
      this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    }
    else
    {
      this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
    }
  }

}
