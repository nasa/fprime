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

#include "Svc/BufferLogger/BufferLogger.hpp"

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  BufferLogger ::
#if FW_OBJECT_NAMES == 1
      BufferLogger(const char *const compName) :
          BufferLoggerComponentBase(compName), //!< The component name
#else
      BufferLogger() : BufferLoggerComponentBase(),
#endif
      m_state(LOGGING_OFF),
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
        Fw::Buffer fwBuffer
    )
  {
    if (m_state == LOGGING_ON) {
      const U8 *const addr = reinterpret_cast<U8*>(fwBuffer.getdata());
      const U32 size = fwBuffer.getsize();
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
    if (m_state == LOGGING_ON) {
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

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void BufferLogger ::
    BL_OpenFile_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CmdStringArg& file
    )
  {
    m_file.closeAndEmitEvent();
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
  }

  void BufferLogger ::
    BL_CloseFile_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    m_file.closeAndEmitEvent();
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
  }

  void BufferLogger ::
    BL_SetLogging_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        LogState state
  )
  {
    m_state = state;
    if (state == LOGGING_OFF) {
      m_file.closeAndEmitEvent();
    }
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
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
      this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
    }
    else
    {
      this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_EXECUTION_ERROR);
    }
  }

};
