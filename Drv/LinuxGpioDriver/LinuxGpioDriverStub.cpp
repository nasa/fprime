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


#include <Drv/LinuxGpioDriver/LinuxGpioDriver.hpp>
#include <FpConfig.hpp>

namespace Drv {

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

Os::File::Status LinuxGpioDriver ::open(const char* chip, FwSizeType gpio, GpioDirection direction) {
    return Os::File::Status::NOT_SUPPORTED;
}

  void LinuxGpioDriver ::
    gpioRead_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Logic &state
    )
  {}

  void LinuxGpioDriver ::
    gpioWrite_handler(
        const NATIVE_INT_TYPE portNum,
        const Fw::Logic& state
    )
  {}



  Os::Task::Status LinuxGpioDriver ::
    startIntTask(Os::Task::ParamType priority, Os::Task::ParamType stackSize, Os::Task::ParamType cpuAffinity) {
     return Os::Task::OP_OK;
   }

   LinuxGpioDriver ::
    ~LinuxGpioDriver()
  {

  }

  void LinuxGpioDriver ::
    exitThread() {
  }
} // end namespace Drv
