// ======================================================================
// \title  Example/test/ut/TesterBase.hpp
// \author Auto-generated
// \brief  hpp file for Example component test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Example_TESTER_BASE_HPP
#define Example_TESTER_BASE_HPP

#include <Autocoders/Python/test/noargport/ExampleComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <stdio.h>
#include <Fw/Port/InputSerializePort.hpp>

namespace ExampleComponents {

  //! \class ExampleTesterBase
  //! \brief Auto-generated base class for Example component test harness
  //!
  class ExampleTesterBase :
    public Fw::PassiveComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Initialization
      // ----------------------------------------------------------------------

      //! Initialize object ExampleTesterBase
      //!
      virtual void init(
          const NATIVE_INT_TYPE instance = 0 //!< The instance number
      );

    public:

      // ----------------------------------------------------------------------
      // Connectors for 'to' ports
      // Connect these output ports to the input ports under test
      // ----------------------------------------------------------------------

      //! Connect exampleInput to to_exampleInput[portNum]
      //!
      void connect_to_exampleInput(
          const NATIVE_INT_TYPE portNum, //!< The port number
          APortCollection::InputSomePort *const exampleInput //!< The port
      );

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object ExampleTesterBase
      //!
      ExampleTesterBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, //!< The component name
          const U32 maxHistorySize //!< The maximum size of each history
#else
          const U32 maxHistorySize //!< The maximum size of each history
#endif
      );

      //! Destroy object ExampleTesterBase
      //!
      virtual ~ExampleTesterBase(void);

    protected:

      // ----------------------------------------------------------------------
      // Invocation functions for to ports
      // ----------------------------------------------------------------------

      //! Invoke the to port connected to exampleInput
      //!
      U32 invoke_to_exampleInput(
          const NATIVE_INT_TYPE portNum //!< The port number
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for port counts
      // ----------------------------------------------------------------------

      //! Get the number of to_exampleInput ports
      //!
      //! \return The number of to_exampleInput ports
      //!
      NATIVE_INT_TYPE getNum_to_exampleInput(void) const;

    protected:

      // ----------------------------------------------------------------------
      // Connection status for to ports
      // ----------------------------------------------------------------------

      //! Check whether port is connected
      //!
      //! Whether to_exampleInput[portNum] is connected
      //!
      bool isConnected_to_exampleInput(
          const NATIVE_INT_TYPE portNum //!< The port number
      );

    private:

      // ----------------------------------------------------------------------
      // To ports
      // ----------------------------------------------------------------------

      //! To port connected to exampleInput
      //!
      APortCollection::OutputSomePort m_to_exampleInput[1];

  };

} // end namespace ExampleComponents

#endif
