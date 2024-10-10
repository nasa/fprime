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

LinuxGpioDriver ::~LinuxGpioDriver() {}


// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

Os::File::Status LinuxGpioDriver ::open(const char* device, U32 gpio, GpioConfiguration configuration, Fw::Logic default_state) {
    return Os::File::Status::NOT_SUPPORTED;
}

void LinuxGpioDriver ::gpioRead_handler(const NATIVE_INT_TYPE portNum, Fw::Logic &state) {}

void LinuxGpioDriver ::gpioWrite_handler(const NATIVE_INT_TYPE portNum, const Fw::Logic& state) {}



} // end namespace Drv
