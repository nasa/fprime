// ======================================================================
// \title  LinuxGpioDriverImpl.cpp
// \author tcanham
// \brief  cpp file for LinuxGpioDriver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <Drv/LinuxGpioDriver/LinuxGpioDriverComponentImpl.hpp>
#include <FpConfig.hpp>

namespace Drv {

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void LinuxGpioDriverComponentImpl ::
    gpioRead_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Logic &state
    )
  {
    // TODO
  }

  void LinuxGpioDriverComponentImpl ::
    gpioWrite_handler(
        const NATIVE_INT_TYPE portNum,
        const Fw::Logic& state
    )
  {
    // TODO
  }

  bool LinuxGpioDriverComponentImpl ::
    open(NATIVE_INT_TYPE gpio, GpioDirection direction) {
      return false;
  }

  Os::Task::Status LinuxGpioDriverComponentImpl ::
    startIntTask(Os::Task::ParamType priority, Os::Task::ParamType stackSize, Os::Task::ParamType cpuAffinity) {
     return Os::Task::OP_OK;
   }

  LinuxGpioDriverComponentImpl ::
    ~LinuxGpioDriverComponentImpl()
  {

  }

  void LinuxGpioDriverComponentImpl ::
    exitThread() {
  }
} // end namespace Drv
