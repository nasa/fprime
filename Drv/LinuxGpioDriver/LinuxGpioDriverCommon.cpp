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
#include <Fw/FPrimeBasicTypes.hpp>

namespace Drv {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

LinuxGpioDriver ::LinuxGpioDriver(const char* const compName) : LinuxGpioDriverComponentBase(compName) {}

Drv::GpioStatus LinuxGpioDriver ::start(const FwTaskPriorityType priority,
                                        const FwSizeType stackSize,
                                        const FwSizeType cpuAffinity,
                                        const FwTaskIdType identifier) {
    Drv::GpioStatus status = Drv::GpioStatus::INVALID_MODE;
    if (this->m_configuration < GpioConfiguration::MAX_GPIO_CONFIGURATION &&
        this->m_configuration >= GpioConfiguration::GPIO_INTERRUPT_RISING_EDGE) {
        status = Drv::GpioStatus::OP_OK;
        {
            Os::ScopeLock lock(m_lock);
            this->m_running = true;
        }
        Fw::String name;
        (void)name.format("%s.interrupt", FW_OPTIONAL_NAME(this->getObjName()));  // task name may safely truncate
        Os::Task::Arguments arguments(name, &this->interruptFunction, this, priority, stackSize, cpuAffinity,
                                      identifier);
        Os::Task::Status taskStatus = this->m_poller.start(arguments);
        if (taskStatus != Os::Task::Status::OP_OK) {
            status = Drv::GpioStatus::UNKNOWN_ERROR;
        }
    }
    return status;
}

void LinuxGpioDriver ::stop() {
    Os::ScopeLock lock(m_lock);
    this->m_running = false;
}

void LinuxGpioDriver ::join() {
    (void)this->m_poller.join();  // best-effort join on shutdown
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
