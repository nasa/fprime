// ======================================================================
// \title Os/Stub/RawTime.cpp
// \brief stub implementation for Os::RawTime
// ======================================================================
#include "Os/Stub/RawTime.hpp"

namespace Os {
namespace Stub {
namespace RawTime {

    StubRawTime::Status StubRawTime::getRawTime() {
        return Status::OP_OK;
    }

    StubRawTime::Status StubRawTime::getTimeInterval(const RawTimeHandle& other, Fw::TimeInterval& interval) const {
        interval.set(0, 0);
        return Status::OP_OK;
    }

    Fw::SerializeStatus StubRawTime::serialize(Fw::SerializeBufferBase& buffer) const {
        return Fw::FW_SERIALIZE_OK;
    }

    Fw::SerializeStatus StubRawTime::deserialize(Fw::SerializeBufferBase& buffer) {
        return Fw::FW_SERIALIZE_OK;
    }
} // namespace RawTime
} // namespace Stub
} // namespace Os
