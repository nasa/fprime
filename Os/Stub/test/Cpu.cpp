// ======================================================================
// \title Os/Stub/Cpu.cpp
// \brief stub implementation for Os::Cpu
// ======================================================================
#include <Os/Stub/test/Cpu.hpp>

namespace Os {
namespace Stub {
namespace Cpu {
namespace Test {

StaticData StaticData::data;

TestCpu::TestCpu() {
    StaticData::data.lastCalled = StaticData::LastFn::CONSTRUCT_FN;
}

TestCpu::~TestCpu() {
    StaticData::data.lastCalled = StaticData::LastFn::DESTRUCT_FN;
}


TestCpu::Status TestCpu::_getCount(FwSizeType& cpu_count) {
    StaticData::data.lastCalled = StaticData::LastFn::COUNT_FN;
    StaticData::data.count = cpu_count;
    return  StaticData::data.status_out;
}

TestCpu::Status TestCpu::_getTicks(Os::Cpu::Ticks& ticks, FwSizeType cpu_index) {
    StaticData::data.lastCalled = StaticData::LastFn::TICKS_FN;
    StaticData::data.index = cpu_index;
    StaticData::data.ticks.total = ticks.total;
    StaticData::data.ticks.used = ticks.used;
    return  StaticData::data.status_out;
}

CpuHandle* TestCpu::getHandle() {
    StaticData::data.lastCalled = StaticData::LastFn::HANDLE_FN;
    return &this->m_handle;
}

} // namespace Test
} // namespace Cpu
} // namespace Stub
} // namespace Os
