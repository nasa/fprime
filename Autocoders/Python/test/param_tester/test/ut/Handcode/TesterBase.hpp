// ======================================================================
// \title  ParamTester/test/ut/TesterBase.hpp
// \author Auto-generated
// \brief  hpp file for ParamTester component test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef ParamTester_TESTER_BASE_HPP
#define ParamTester_TESTER_BASE_HPP

#include <Autocoders/Python/test/param_tester/ParamTestComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <stdio.h>
#include <Fw/Port/InputSerializePort.hpp>

namespace Prm {

  //! \class ParamTesterTesterBase
  //! \brief Auto-generated base class for ParamTester component test harness
  //!
  class ParamTesterTesterBase :
    public Fw::PassiveComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Initialization
      // ----------------------------------------------------------------------

      //! Initialize object ParamTesterTesterBase
      //!
      virtual void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

    public:

      // ----------------------------------------------------------------------
      // Connectors for 'to' ports
      // Connect these output ports to the input ports under test
      // ----------------------------------------------------------------------

      //! Connect paramGetPort to to_paramGetPort[portNum]
      //!
      void connect_to_paramGetPort(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::InputPrmGetPort *const paramGetPort /*!< The port*/
      );

      //! Connect paramSetPort to to_paramSetPort[portNum]
      //!
      void connect_to_paramSetPort(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::InputPrmSetPort *const paramSetPort /*!< The port*/
      );

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object ParamTesterTesterBase
      //!
      ParamTesterTesterBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object ParamTesterTesterBase
      //!
      virtual ~ParamTesterTesterBase(void);

    protected:

      // ----------------------------------------------------------------------
      // Invocation functions for to ports
      // ----------------------------------------------------------------------

      //! Invoke the to port connected to paramGetPort
      //!
      Fw::ParamValid invoke_to_paramGetPort(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwPrmIdType id, /*!< Parameter ID*/
          Fw::ParamBuffer &val /*!< Buffer containing serialized parameter value*/
      );

      //! Invoke the to port connected to paramSetPort
      //!
      void invoke_to_paramSetPort(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwPrmIdType id, /*!< Parameter ID*/
          Fw::ParamBuffer &val /*!< Buffer containing serialized parameter value*/
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for port counts
      // ----------------------------------------------------------------------

      //! Get the number of to_paramGetPort ports
      //!
      //! \return The number of to_paramGetPort ports
      //!
      NATIVE_INT_TYPE getNum_to_paramGetPort(void) const;

      //! Get the number of to_paramSetPort ports
      //!
      //! \return The number of to_paramSetPort ports
      //!
      NATIVE_INT_TYPE getNum_to_paramSetPort(void) const;

    protected:

      // ----------------------------------------------------------------------
      // Connection status for to ports
      // ----------------------------------------------------------------------

      //! Check whether port is connected
      //!
      //! Whether to_paramGetPort[portNum] is connected
      //!
      bool isConnected_to_paramGetPort(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Check whether port is connected
      //!
      //! Whether to_paramSetPort[portNum] is connected
      //!
      bool isConnected_to_paramSetPort(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

    private:

      // ----------------------------------------------------------------------
      // To ports
      // ----------------------------------------------------------------------

      //! To port connected to paramGetPort
      //!
      Fw::OutputPrmGetPort m_to_paramGetPort[1];

      //! To port connected to paramSetPort
      //!
      Fw::OutputPrmSetPort m_to_paramSetPort[1];

  };

} // end namespace Prm

#endif
