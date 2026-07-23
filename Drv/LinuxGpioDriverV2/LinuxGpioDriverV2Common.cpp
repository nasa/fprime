// ======================================================================
// \title  LinuxGpioDriverV2Common.cpp
// \brief  cpp file for LinuxGpioDriverV2 component common implementation
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Drv/LinuxGpioDriverV2/LinuxGpioDriverV2.hpp>
#include <Fw/FPrimeBasicTypes.hpp>

namespace Drv {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

LinuxGpioDriverV2 ::LinuxGpioDriverV2(const char* const compName) : LinuxGpioDriverV2ComponentBase(compName) {}

Drv::GpioStatus LinuxGpioDriverV2 ::start(const FwTaskPriorityType priority,
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

void LinuxGpioDriverV2 ::stop() {
    Os::ScopeLock lock(m_lock);
    this->m_running = false;
}

void LinuxGpioDriverV2 ::join() {
    (void)this->m_poller.join();  // best-effort join on shutdown
}

void LinuxGpioDriverV2 ::interruptFunction(void* self) {
    FW_ASSERT(self != nullptr);
    LinuxGpioDriverV2* component = reinterpret_cast<LinuxGpioDriverV2*>(self);
    component->pollLoop();
}

bool LinuxGpioDriverV2 ::getRunning() {
    Os::ScopeLock lock(m_lock);
    return this->m_running;
}

}  // end namespace Drv
