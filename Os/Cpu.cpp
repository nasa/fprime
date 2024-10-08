// ======================================================================
// \title Os/Cpu.hpp
// \brief common function implementations for Os::Cpu
// ======================================================================
#include "Os/Cpu.hpp"
#include "Fw/Types/Assert.hpp"

namespace Os {

Cpu::Cpu() : m_delegate(*CpuInterface::getDelegate(m_handle_storage)) {}

Cpu::~Cpu() {
    m_delegate.~CpuInterface();
}

void Cpu::init() {
    (void) Cpu::getSingleton();
}

Cpu& Cpu::getSingleton() {
    static Cpu _singleton;
    return _singleton;
}

Cpu::Status Cpu::_getCount(FwSizeType& cpu_count) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<CpuInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate._getCount(cpu_count);
}

Cpu::Status Cpu::_getTicks(Ticks& ticks, FwSizeType cpu_index) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<CpuInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate._getTicks(ticks, cpu_index);
}

CpuHandle* Cpu::getHandle() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<CpuInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.getHandle();
}

Cpu::Status Cpu::getCount(FwSizeType& cpu_count) {
    return Cpu::getSingleton()._getCount(cpu_count);
}

Cpu::Status Cpu::getTicks(Ticks& ticks, FwSizeType cpu_index) {
    return Cpu::getSingleton()._getTicks(ticks, cpu_index);
}
}
