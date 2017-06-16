// ======================================================================
// \title  LogTester/test/ut/TesterBase.hpp
// \author Auto-generated
// \brief  hpp file for LogTester component test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
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

#ifndef LogTester_TESTER_BASE_HPP
#define LogTester_TESTER_BASE_HPP

#include <Autocoders/test/log_tester/LogTestComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <stdio.h>
#include <Fw/Port/InputSerializePort.hpp>

namespace Log {

  //! \class LogTesterTesterBase
  //! \brief Auto-generated base class for LogTester component test harness
  //!
  class LogTesterTesterBase :
    public Fw::PassiveComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Initialization
      // ----------------------------------------------------------------------

      //! Initialize object LogTesterTesterBase
      //!
      virtual void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

    public:

      // ----------------------------------------------------------------------
      // Connectors for 'to' ports
      // Connect these output ports to the input ports under test
      // ----------------------------------------------------------------------

      //! Connect logRecvPort to to_logRecvPort[portNum]
      //!
      void connect_to_logRecvPort(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::InputLogPort *const logRecvPort /*!< The port*/
      );

      //! Connect textLogRecvPort to to_textLogRecvPort[portNum]
      //!
      void connect_to_textLogRecvPort(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::InputLogTextPort *const textLogRecvPort /*!< The port*/
      );

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object LogTesterTesterBase
      //!
      LogTesterTesterBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object LogTesterTesterBase
      //!
      virtual ~LogTesterTesterBase(void);

    protected:

      // ----------------------------------------------------------------------
      // Invocation functions for to ports
      // ----------------------------------------------------------------------

      //! Invoke the to port connected to logRecvPort
      //!
      void invoke_to_logRecvPort(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwEventIdType id, /*!< Log ID*/
          Fw::Time &timeTag, /*!< Time Tag*/
          Fw::LogSeverity severity, /*!< The severity argument*/
          Fw::LogBuffer &args /*!< Buffer containing serialized log entry*/
      );

      //! Invoke the to port connected to textLogRecvPort
      //!
      void invoke_to_textLogRecvPort(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwEventIdType id, /*!< Log ID*/
          Fw::Time &timeTag, /*!< Time Tag*/
          Fw::TextLogSeverity severity, /*!< The severity argument*/
          Fw::TextLogString &text /*!< Text of log message*/
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for port counts
      // ----------------------------------------------------------------------

      //! Get the number of to_logRecvPort ports
      //!
      //! \return The number of to_logRecvPort ports
      //!
      NATIVE_INT_TYPE getNum_to_logRecvPort(void) const;

      //! Get the number of to_textLogRecvPort ports
      //!
      //! \return The number of to_textLogRecvPort ports
      //!
      NATIVE_INT_TYPE getNum_to_textLogRecvPort(void) const;

    protected:

      // ----------------------------------------------------------------------
      // Connection status for to ports
      // ----------------------------------------------------------------------

      //! Check whether port is connected
      //!
      //! Whether to_logRecvPort[portNum] is connected
      //!
      bool isConnected_to_logRecvPort(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Check whether port is connected
      //!
      //! Whether to_textLogRecvPort[portNum] is connected
      //!
      bool isConnected_to_textLogRecvPort(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

    private:

      // ----------------------------------------------------------------------
      // To ports
      // ----------------------------------------------------------------------

      //! To port connected to logRecvPort
      //!
      Fw::OutputLogPort m_to_logRecvPort[1];

      //! To port connected to textLogRecvPort
      //!
      Fw::OutputLogTextPort m_to_textLogRecvPort[1];

  };

} // end namespace Log

#endif
