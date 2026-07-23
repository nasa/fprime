// ======================================================================
// \title  LinuxGpioDriverV2Stub.cpp
// \brief  cpp file for LinuxGpioDriverV2 component stub implementation
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Drv/LinuxGpioDriverV2/LinuxGpioDriverV2.hpp>
#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Time/Time.hpp>

namespace Drv {

LinuxGpioDriverV2 ::~LinuxGpioDriverV2() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

Os::File::Status LinuxGpioDriverV2 ::setupLineRequest(const int chip_descriptor,
                                                      const U32 gpio,
                                                      const GpioConfiguration& configuration,
                                                      const Fw::Logic& default_state,
                                                      int& fd) {
    return Os::File::Status::NOT_SUPPORTED;
}

Os::File::Status LinuxGpioDriverV2 ::open(const char* device,
                                          const U32 gpio,
                                          const GpioConfiguration& configuration,
                                          const Fw::Logic& default_state) {
    return Os::File::Status::NOT_SUPPORTED;
}

Drv::GpioStatus LinuxGpioDriverV2 ::gpioRead_handler(const FwIndexType portNum, Fw::Logic& state) {
    return Drv::GpioStatus::UNKNOWN_ERROR;
}

Drv::GpioStatus LinuxGpioDriverV2 ::gpioWrite_handler(const FwIndexType portNum, const Fw::Logic& state) {
    return Drv::GpioStatus::UNKNOWN_ERROR;
}

void LinuxGpioDriverV2 ::pollLoop() {
    // Loop forever
    while (this->getRunning()) {
        Os::Task::delay(Fw::TimeInterval(GPIO_POLL_TIMEOUT / 1000, (GPIO_POLL_TIMEOUT % 1000) * 1000));
    }
}

}  // end namespace Drv
