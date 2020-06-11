// ======================================================================
// \title  ActiveTextLogger/test/ut/TesterBase.hpp
// \author Auto-generated
// \brief  hpp file for ActiveTextLogger component test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#ifndef ActiveTextLogger_TESTER_BASE_HPP
#define ActiveTextLogger_TESTER_BASE_HPP

#include <Svc/ActiveTextLogger/ActiveTextLoggerComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <stdio.h>
#include <Fw/Port/InputSerializePort.hpp>

namespace Svc {

  //! \class ActiveTextLoggerTesterBase
  //! \brief Auto-generated base class for ActiveTextLogger component test harness
  //!
  class ActiveTextLoggerTesterBase :
    public Fw::PassiveComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Initialization
      // ----------------------------------------------------------------------

      //! Initialize object ActiveTextLoggerTesterBase
      //!
      virtual void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

    public:

      // ----------------------------------------------------------------------
      // Connectors for 'to' ports
      // Connect these output ports to the input ports under test
      // ----------------------------------------------------------------------

      //! Connect TextLogger to to_TextLogger[portNum]
      //!
      void connect_to_TextLogger(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::InputLogTextPort *const TextLogger /*!< The port*/
      );

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object ActiveTextLoggerTesterBase
      //!
      ActiveTextLoggerTesterBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object ActiveTextLoggerTesterBase
      //!
      virtual ~ActiveTextLoggerTesterBase(void);

    protected:

      // ----------------------------------------------------------------------
      // Invocation functions for to ports
      // ----------------------------------------------------------------------

      //! Invoke the to port connected to TextLogger
      //!
      void invoke_to_TextLogger(
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

      //! Get the number of to_TextLogger ports
      //!
      //! \return The number of to_TextLogger ports
      //!
      NATIVE_INT_TYPE getNum_to_TextLogger(void) const;

    protected:

      // ----------------------------------------------------------------------
      // Connection status for to ports
      // ----------------------------------------------------------------------

      //! Check whether port is connected
      //!
      //! Whether to_TextLogger[portNum] is connected
      //!
      bool isConnected_to_TextLogger(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

    private:

      // ----------------------------------------------------------------------
      // To ports
      // ----------------------------------------------------------------------

      //! To port connected to TextLogger
      //!
      Fw::OutputLogTextPort m_to_TextLogger[1];

  };

} // end namespace Svc

#endif
