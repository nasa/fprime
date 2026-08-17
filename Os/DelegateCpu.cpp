// ======================================================================
// \title Os/DelegateCpu.cpp
// \brief common function implementations for Os::DelegateCpu
// ======================================================================
#include "Os/DelegateCpu.hpp"
#include "Fw/Types/Assert.hpp"

namespace Os {

DelegateCpu::DelegateCpu() : m_handle_storage(), m_delegate(*CpuInterface::getDelegate(m_handle_storage)) {}

DelegateCpu::~DelegateCpu() {
    m_delegate.~CpuInterface();
}

void DelegateCpu::init() {
    (void)DelegateCpu::getSingleton();
}

DelegateCpu& DelegateCpu::getSingleton() {
    static DelegateCpu _singleton;
    return _singleton;
}

DelegateCpu::Status DelegateCpu::_getCount(FwSizeType& cpu_count) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<CpuInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate._getCount(cpu_count);
}

DelegateCpu::Status DelegateCpu::_getTicks(Ticks& ticks, FwSizeType cpu_index) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<CpuInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate._getTicks(ticks, cpu_index);
}

CpuHandle* DelegateCpu::getHandle() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<CpuInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.getHandle();
}
}  // namespace Os
