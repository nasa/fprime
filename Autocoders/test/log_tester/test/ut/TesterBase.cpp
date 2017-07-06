// ======================================================================
// \title  LogTester/test/ut/TesterBase.cpp
// \author Auto-generated
// \brief  cpp file for LogTester component test harness base class
//
// \copyright
// Copyright 2009-2016, by the California Institute of Technology.
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

#include <stdlib.h>
#include <string.h>
#include "TesterBase.hpp"

namespace Log {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  LogTesterTesterBase ::
    LogTesterTesterBase(
#if FW_OBJECT_NAMES == 1
        const char *const compName,
        const U32 maxHistorySize
#else
        const U32 maxHistorySize
#endif
    ) :
#if FW_OBJECT_NAMES == 1
      Fw::PassiveComponentBase(compName)
#else
      Fw::PassiveComponentBase()
#endif
  {
  }

  LogTesterTesterBase ::
    ~LogTesterTesterBase(void) 
  {
  }

  void LogTesterTesterBase ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {

    // Initialize base class

		Fw::PassiveComponentBase::init(instance);

    // Initialize output port logRecvPort

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_logRecvPort();
        ++_port
    ) {
      this->m_to_logRecvPort[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_logRecvPort[%d]",
          this->m_objName,
          _port
      );
      this->m_to_logRecvPort[_port].setObjName(_portName);
#endif

    }

    // Initialize output port textLogRecvPort

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_textLogRecvPort();
        ++_port
    ) {
      this->m_to_textLogRecvPort[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_textLogRecvPort[%d]",
          this->m_objName,
          _port
      );
      this->m_to_textLogRecvPort[_port].setObjName(_portName);
#endif

    }

  }

  // ----------------------------------------------------------------------
  // Getters for port counts
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE LogTesterTesterBase ::
    getNum_to_logRecvPort(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_logRecvPort);
  }

  NATIVE_INT_TYPE LogTesterTesterBase ::
    getNum_to_textLogRecvPort(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_textLogRecvPort);
  }

  // ----------------------------------------------------------------------
  // Connectors for to ports 
  // ----------------------------------------------------------------------

  void LogTesterTesterBase ::
    connect_to_logRecvPort(
        const NATIVE_INT_TYPE portNum,
        Fw::InputLogPort *const logRecvPort
    ) 
  {
    FW_ASSERT(portNum < this->getNum_to_logRecvPort(),static_cast<AssertArg>(portNum));
    this->m_to_logRecvPort[portNum].addCallPort(logRecvPort);
  }

  void LogTesterTesterBase ::
    connect_to_textLogRecvPort(
        const NATIVE_INT_TYPE portNum,
        Fw::InputLogTextPort *const textLogRecvPort
    ) 
  {
    FW_ASSERT(portNum < this->getNum_to_textLogRecvPort(),static_cast<AssertArg>(portNum));
    this->m_to_textLogRecvPort[portNum].addCallPort(textLogRecvPort);
  }


  // ----------------------------------------------------------------------
  // Invocation functions for to ports
  // ----------------------------------------------------------------------

  void LogTesterTesterBase ::
    invoke_to_logRecvPort(
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::LogSeverity severity,
        Fw::LogBuffer &args
    )
  {
    FW_ASSERT(portNum < this->getNum_to_logRecvPort(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_logRecvPort(),static_cast<AssertArg>(portNum));
    this->m_to_logRecvPort[portNum].invoke(
        id, timeTag, severity, args
    );
  }

  void LogTesterTesterBase ::
    invoke_to_textLogRecvPort(
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::TextLogSeverity severity,
        Fw::TextLogString &text
    )
  {
    FW_ASSERT(portNum < this->getNum_to_textLogRecvPort(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_textLogRecvPort(),static_cast<AssertArg>(portNum));
    this->m_to_textLogRecvPort[portNum].invoke(
        id, timeTag, severity, text
    );
  }

  // ----------------------------------------------------------------------
  // Connection status for to ports
  // ----------------------------------------------------------------------

  bool LogTesterTesterBase ::
    isConnected_to_logRecvPort(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_logRecvPort(), static_cast<AssertArg>(portNum));
    return this->m_to_logRecvPort[portNum].isConnected();
  }

  bool LogTesterTesterBase ::
    isConnected_to_textLogRecvPort(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_textLogRecvPort(), static_cast<AssertArg>(portNum));
    return this->m_to_textLogRecvPort[portNum].isConnected();
  }

} // end namespace Log
