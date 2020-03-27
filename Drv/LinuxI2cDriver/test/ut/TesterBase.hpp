// ======================================================================
// \title  LinuxI2cDriver/test/ut/TesterBase.hpp
// \author Auto-generated
// \brief  hpp file for LinuxI2cDriver component test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef LinuxI2cDriver_TESTER_BASE_HPP
#define LinuxI2cDriver_TESTER_BASE_HPP

#include <Drv/LinuxI2cDriver/LinuxI2cDriverComponentAc.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>
#include <stdio.h>
#include <Fw/Port/InputSerializePort.hpp>

namespace Drv {

  //! \class LinuxI2cDriverTesterBase
  //! \brief Auto-generated base class for LinuxI2cDriver component test harness
  //!
  class LinuxI2cDriverTesterBase :
    public Fw::PassiveComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Initialization
      // ----------------------------------------------------------------------

      //! Initialize object LinuxI2cDriverTesterBase
      //!
      virtual void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

    public:

      // ----------------------------------------------------------------------
      // Connectors for 'to' ports
      // Connect these output ports to the input ports under test
      // ----------------------------------------------------------------------

      //! Connect write to to_write[portNum]
      //!
      void connect_to_write(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Drv::InputI2cWritePort *const write /*!< The port*/
      );

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object LinuxI2cDriverTesterBase
      //!
      LinuxI2cDriverTesterBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object LinuxI2cDriverTesterBase
      //!
      virtual ~LinuxI2cDriverTesterBase(void);

    protected:

      // ----------------------------------------------------------------------
      // Invocation functions for to ports
      // ----------------------------------------------------------------------

      //! Invoke the to port connected to write
      //!
      void invoke_to_write(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 addr, 
          Fw::Buffer &serBuffer 
      );

    public:

      // ----------------------------------------------------------------------
      // Getters for port counts
      // ----------------------------------------------------------------------

      //! Get the number of to_write ports
      //!
      //! \return The number of to_write ports
      //!
      NATIVE_INT_TYPE getNum_to_write(void) const;

    protected:

      // ----------------------------------------------------------------------
      // Connection status for to ports
      // ----------------------------------------------------------------------

      //! Check whether port is connected
      //!
      //! Whether to_write[portNum] is connected
      //!
      bool isConnected_to_write(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

    private:

      // ----------------------------------------------------------------------
      // To ports
      // ----------------------------------------------------------------------

      //! To port connected to write
      //!
      Drv::OutputI2cWritePort m_to_write[1];

  };

} // end namespace Drv

#endif
