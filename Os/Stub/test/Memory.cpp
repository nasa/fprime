// ======================================================================
// \title Os/Stub/Memory.cpp
// \brief stub implementation for Os::Memory
// ======================================================================
#include <Os/Stub/test/Memory.hpp>

namespace Os {
namespace Stub {
namespace Memory {
namespace Test {

StaticData StaticData::data;

TestMemory::TestMemory() {
    StaticData::data.lastCalled = StaticData::LastFn::CONSTRUCT_FN;
}

TestMemory::~TestMemory() {
    StaticData::data.lastCalled = StaticData::LastFn::DESTRUCT_FN;
}

TestMemory::Status TestMemory::_getUsage(Os::Memory::Usage& memory_usage) {
    StaticData::data.lastCalled = StaticData::LastFn::USAGE_FN;
    StaticData::data.usage.total = memory_usage.total;
    StaticData::data.usage.used = memory_usage.used;
    return Status::ERROR;
}


MemoryHandle* TestMemory::getHandle() {
    StaticData::data.lastCalled = StaticData::LastFn::HANDLE_FN;
    return &this->m_handle;
}

} // namespace Test
} // namespace Memory
} // namespace Stub
} // namespace Os
