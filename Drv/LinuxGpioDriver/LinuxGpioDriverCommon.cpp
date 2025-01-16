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
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

LinuxGpioDriver ::LinuxGpioDriver(const char* const compName) : LinuxGpioDriverComponentBase(compName) {}

Drv::GpioStatus LinuxGpioDriver ::start(const FwSizeType priority,
                                        const FwSizeType stackSize,
                                        const FwSizeType cpuAffinity,
                                        const PlatformUIntType identifier) {
    Drv::GpioStatus status = Drv::GpioStatus::INVALID_MODE;
    if (this->m_configuration < GpioConfiguration::MAX_GPIO_CONFIGURATION &&
        this->m_configuration >= GpioConfiguration::GPIO_INTERRUPT_RISING_EDGE) {
        status = Drv::GpioStatus::OP_OK;
        {
            Os::ScopeLock lock(m_lock);
            this->m_running = true;
        }
        Fw::String name;
        name.format("%s.interrupt", FW_OPTIONAL_NAME(this->getObjName()));
        Os::Task::Arguments arguments(name, &this->interruptFunction, this, priority, stackSize, cpuAffinity,
                                      identifier);
        this->m_poller.start(arguments);
    }
    return status;
}

void LinuxGpioDriver ::stop() {
    Os::ScopeLock lock(m_lock);
    this->m_running = false;
}

void LinuxGpioDriver ::join() {
    this->m_poller.join();
}

void LinuxGpioDriver ::interruptFunction(void* self) {
    FW_ASSERT(self != nullptr);
    LinuxGpioDriver* component = reinterpret_cast<LinuxGpioDriver*>(self);
    component->pollLoop();
}

bool LinuxGpioDriver ::getRunning() {
    Os::ScopeLock lock(m_lock);
    return this->m_running;
}

}  // end namespace Drv
