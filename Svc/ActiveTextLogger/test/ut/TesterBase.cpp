// ======================================================================
// \title  ActiveTextLogger/test/ut/TesterBase.cpp
// \author Auto-generated
// \brief  cpp file for ActiveTextLogger component test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#include <stdlib.h>
#include <string.h>
#include "TesterBase.hpp"

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  ActiveTextLoggerTesterBase ::
    ActiveTextLoggerTesterBase(
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

  ActiveTextLoggerTesterBase ::
    ~ActiveTextLoggerTesterBase(void) 
  {
  }

  void ActiveTextLoggerTesterBase ::
    init(NATIVE_INT_TYPE instance)
  {

    // Initialize base class

		Fw::PassiveComponentBase::init(instance);

    // Initialize output port TextLogger

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_TextLogger();
        ++_port
    ) {
      this->m_to_TextLogger[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_TextLogger[%d]",
          this->m_objName,
          _port
      );
      this->m_to_TextLogger[_port].setObjName(_portName);
#endif

    }

  }

  // ----------------------------------------------------------------------
  // Getters for port counts
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE ActiveTextLoggerTesterBase ::
    getNum_to_TextLogger(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_TextLogger);
  }

  // ----------------------------------------------------------------------
  // Connectors for to ports 
  // ----------------------------------------------------------------------

  void ActiveTextLoggerTesterBase ::
    connect_to_TextLogger(
        const NATIVE_INT_TYPE portNum,
        Fw::InputLogTextPort *const TextLogger
    ) 
  {
    FW_ASSERT(portNum < this->getNum_to_TextLogger(),static_cast<AssertArg>(portNum));
    this->m_to_TextLogger[portNum].addCallPort(TextLogger);
  }


  // ----------------------------------------------------------------------
  // Invocation functions for to ports
  // ----------------------------------------------------------------------

  void ActiveTextLoggerTesterBase ::
    invoke_to_TextLogger(
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::TextLogSeverity severity,
        Fw::TextLogString &text
    )
  {
    FW_ASSERT(portNum < this->getNum_to_TextLogger(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_TextLogger(),static_cast<AssertArg>(portNum));
    this->m_to_TextLogger[portNum].invoke(
        id, timeTag, severity, text
    );
  }

  // ----------------------------------------------------------------------
  // Connection status for to ports
  // ----------------------------------------------------------------------

  bool ActiveTextLoggerTesterBase ::
    isConnected_to_TextLogger(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_TextLogger(), static_cast<AssertArg>(portNum));
    return this->m_to_TextLogger[portNum].isConnected();
  }

} // end namespace Svc
