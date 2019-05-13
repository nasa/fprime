// ======================================================================
// \title  Time/test/ut/TesterBase.hpp
// \author Auto-generated
// \brief  hpp file for Time component test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Time_TESTER_BASE_HPP
#define Time_TESTER_BASE_HPP

#include <Svc/Time/TimeComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <stdio.h>
#include <Fw/Port/InputSerializePort.hpp>

namespace Svc {

  //! \class TimeTesterBase
  //! \brief Auto-generated base class for Time component test harness
  //!
  class TimeTesterBase :
    public Fw::PassiveComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Initialization
      // ----------------------------------------------------------------------

      //! Initialize object TimeTesterBase
      //!
      virtual void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

    public:

      // ----------------------------------------------------------------------
      // Connectors for 'to' ports
      // Connect these output ports to the input ports under test
      // ----------------------------------------------------------------------

      //! Connect timeGetPort to to_timeGetPort[portNum]
      //!
      void connect_to_timeGetPort(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::InputTimePort *const timeGetPort /*!< The port*/
      );

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object TimeTesterBase
      //!
      TimeTesterBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object TimeTesterBase
      //!
      virtual ~TimeTesterBase(void);

    protected:

      // ----------------------------------------------------------------------
      // Invocation functions for to ports
      // ----------------------------------------------------------------------

      //! Invoke the to port connected to timeGetPort
      //!
      void invoke_to_timeGetPort(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Time &time /*!< The U32 cmd argument*/
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for port counts
      // ----------------------------------------------------------------------

      //! Get the number of to_timeGetPort ports
      //!
      //! \return The number of to_timeGetPort ports
      //!
      NATIVE_INT_TYPE getNum_to_timeGetPort(void) const;

    protected:

      // ----------------------------------------------------------------------
      // Connection status for to ports
      // ----------------------------------------------------------------------

      //! Check whether port is connected
      //!
      //! Whether to_timeGetPort[portNum] is connected
      //!
      bool isConnected_to_timeGetPort(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

    private:

      // ----------------------------------------------------------------------
      // To ports
      // ----------------------------------------------------------------------

      //! To port connected to timeGetPort
      //!
      Fw::OutputTimePort m_to_timeGetPort[1];

  };

} // end namespace Svc

#endif
