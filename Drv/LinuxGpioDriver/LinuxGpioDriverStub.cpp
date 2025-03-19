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
#include <Fw/Time/Time.hpp>

namespace Drv {

LinuxGpioDriver ::~LinuxGpioDriver() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

Os::File::Status LinuxGpioDriver ::setupLineHandle(const PlatformIntType chip_descriptor,
                                                   const U32 gpio,
                                                   const GpioConfiguration& configuration,
                                                   const Fw::Logic& default_state,
                                                   PlatformIntType& fd) {
    return Os::File::Status::NOT_SUPPORTED;
}

Os::File::Status LinuxGpioDriver ::setupLineEvent(const PlatformIntType chip_descriptor,
                                                  const U32 gpio,
                                                  const GpioConfiguration& configuration,
                                                  PlatformIntType& fd) {
    return Os::File::Status::NOT_SUPPORTED;
}

Os::File::Status LinuxGpioDriver ::open(const char* device,
                                        const U32 gpio,
                                        const GpioConfiguration& configuration,
                                        const Fw::Logic& default_state) {
    return Os::File::Status::NOT_SUPPORTED;
}

Drv::GpioStatus LinuxGpioDriver ::gpioRead_handler(const FwIndexType portNum, Fw::Logic& state) {
    return Drv::GpioStatus::UNKNOWN_ERROR;
}

Drv::GpioStatus LinuxGpioDriver ::gpioWrite_handler(const FwIndexType portNum, const Fw::Logic& state) {
    return Drv::GpioStatus::UNKNOWN_ERROR;
}

void LinuxGpioDriver ::pollLoop() {
    // Loop forever
    while (this->getRunning()) {
        Os::Task::delay(Fw::TimeInterval(GPIO_POLL_TIMEOUT / 1000, (GPIO_POLL_TIMEOUT % 1000) * 1000));
    }
}

}  // end namespace Drv
