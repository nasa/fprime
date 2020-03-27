// ======================================================================
// \title  TelemTester/test/ut/TesterBase.hpp
// \author Auto-generated
// \brief  hpp file for TelemTester component test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TelemTester_TESTER_BASE_HPP
#define TelemTester_TESTER_BASE_HPP

#include <Autocoders/Python/test/telem_tester/TelemTestComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <stdio.h>
#include <Fw/Port/InputSerializePort.hpp>

namespace Tlm {

  //! \class TelemTesterTesterBase
  //! \brief Auto-generated base class for TelemTester component test harness
  //!
  class TelemTesterTesterBase :
    public Fw::PassiveComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Initialization
      // ----------------------------------------------------------------------

      //! Initialize object TelemTesterTesterBase
      //!
      virtual void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

    public:

      // ----------------------------------------------------------------------
      // Connectors for 'to' ports
      // Connect these output ports to the input ports under test
      // ----------------------------------------------------------------------

      //! Connect tlmRecvPort to to_tlmRecvPort[portNum]
      //!
      void connect_to_tlmRecvPort(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::InputTlmPort *const tlmRecvPort /*!< The port*/
      );

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object TelemTesterTesterBase
      //!
      TelemTesterTesterBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object TelemTesterTesterBase
      //!
      virtual ~TelemTesterTesterBase(void);

    protected:

      // ----------------------------------------------------------------------
      // Invocation functions for to ports
      // ----------------------------------------------------------------------

      //! Invoke the to port connected to tlmRecvPort
      //!
      void invoke_to_tlmRecvPort(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwChanIdType id, /*!< Telemetry Channel ID*/
          Fw::Time &timeTag, /*!< Time Tag*/
          Fw::TlmBuffer &val /*!< Buffer containing serialized telemetry value*/
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for port counts
      // ----------------------------------------------------------------------

      //! Get the number of to_tlmRecvPort ports
      //!
      //! \return The number of to_tlmRecvPort ports
      //!
      NATIVE_INT_TYPE getNum_to_tlmRecvPort(void) const;

    protected:

      // ----------------------------------------------------------------------
      // Connection status for to ports
      // ----------------------------------------------------------------------

      //! Check whether port is connected
      //!
      //! Whether to_tlmRecvPort[portNum] is connected
      //!
      bool isConnected_to_tlmRecvPort(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

    private:

      // ----------------------------------------------------------------------
      // To ports
      // ----------------------------------------------------------------------

      //! To port connected to tlmRecvPort
      //!
      Fw::OutputTlmPort m_to_tlmRecvPort[1];

  };

} // end namespace Tlm

#endif
