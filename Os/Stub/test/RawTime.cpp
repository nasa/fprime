// ======================================================================
// \title Os/Stub/RawTime.cpp
// \brief stub implementation for Os::RawTime
// ======================================================================
#include "Os/Stub/test/RawTime.hpp"

namespace Os {
namespace Stub {
namespace RawTime {
namespace Test {

StaticData StaticData::data;

TestRawTime::TestRawTime() {
    StaticData::data.lastCalled = StaticData::LastFn::CONSTRUCT_FN;
}

TestRawTime::TestRawTime(const TestRawTime& other) {
    StaticData::data.lastCalled = StaticData::LastFn::COPY_CONSTRUCT_FN;
}

TestRawTime::~TestRawTime() {
    StaticData::data.lastCalled = StaticData::LastFn::DESTRUCT_FN;
}

RawTimeHandle* TestRawTime::getHandle() {
    StaticData::data.lastCalled = StaticData::LastFn::GET_HANDLE_FN;
    return &this->m_handle;
}

TestRawTime::Status TestRawTime::now() {
    StaticData::data.lastCalled = StaticData::LastFn::GET_TIME_FN;
    return Status::OP_OK;
}

TestRawTime::Status TestRawTime::getTimeInterval(const Os::RawTime& other, Fw::TimeInterval& interval) const {
    StaticData::data.lastCalled = StaticData::LastFn::GET_INTERVAL_FN;
    return Status::OP_OK;
}

Fw::SerializeStatus TestRawTime::serialize(Fw::SerializeBufferBase& buffer) const {
    StaticData::data.lastCalled = StaticData::LastFn::SERIALIZE_FN;
    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus TestRawTime::deserialize(Fw::SerializeBufferBase& buffer) {
    StaticData::data.lastCalled = StaticData::LastFn::DESERIALIZE_FN;
    return Fw::FW_SERIALIZE_OK;
}
} // namespace Test
} // namespace RawTime
} // namespace Stub
} // namespace Os
