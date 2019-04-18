// ======================================================================
// \title  PassBy/test/ut/TesterBase.hpp
// \author Auto-generated
// \brief  hpp file for PassBy component test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef PassBy_TESTER_BASE_HPP
#define PassBy_TESTER_BASE_HPP

#include <Autocoders/Python/test/pass_by_attrib/PassByComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <stdio.h>
#include <Fw/Port/InputSerializePort.hpp>

namespace App {

  //! \class PassByTesterBase
  //! \brief Auto-generated base class for PassBy component test harness
  //!
  class PassByTesterBase :
    public Fw::PassiveComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Initialization
      // ----------------------------------------------------------------------

      //! Initialize object PassByTesterBase
      //!
      virtual void init(
          const NATIVE_INT_TYPE instance = 0 //!< The instance number
      );

    public:

      // ----------------------------------------------------------------------
      // Connectors for 'to' ports
      // Connect these output ports to the input ports under test
      // ----------------------------------------------------------------------

      //! Connect ExtIn to to_ExtIn[portNum]
      //!
      void connect_to_ExtIn(
          const NATIVE_INT_TYPE portNum, //!< The port number
          Ports::InputMsg1Port *const ExtIn //!< The port
      );

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object PassByTesterBase
      //!
      PassByTesterBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, //!< The component name
          const U32 maxHistorySize //!< The maximum size of each history
#else
          const U32 maxHistorySize //!< The maximum size of each history
#endif
      );

      //! Destroy object PassByTesterBase
      //!
      virtual ~PassByTesterBase(void);

    protected:

      // ----------------------------------------------------------------------
      // Invocation functions for to ports
      // ----------------------------------------------------------------------

      //! Invoke the to port connected to ExtIn
      //!
      void invoke_to_ExtIn(
          const NATIVE_INT_TYPE portNum, //!< The port number
          U32 arg1, //!< The U32 cmd argument
          U32 *arg2, //!< The U32 cmd argument
          U32 &arg3 //!< The U32 cmd argument
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for port counts
      // ----------------------------------------------------------------------

      //! Get the number of to_ExtIn ports
      //!
      //! \return The number of to_ExtIn ports
      //!
      NATIVE_INT_TYPE getNum_to_ExtIn(void) const;

    protected:

      // ----------------------------------------------------------------------
      // Connection status for to ports
      // ----------------------------------------------------------------------

      //! Check whether port is connected
      //!
      //! Whether to_ExtIn[portNum] is connected
      //!
      bool isConnected_to_ExtIn(
          const NATIVE_INT_TYPE portNum //!< The port number
      );

    private:

      // ----------------------------------------------------------------------
      // To ports
      // ----------------------------------------------------------------------

      //! To port connected to ExtIn
      //!
      Ports::OutputMsg1Port m_to_ExtIn[1];

  };

} // end namespace App

#endif
