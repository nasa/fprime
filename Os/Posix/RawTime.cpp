// ======================================================================
// \title Os/Posix/RawTime.cpp
// \brief Posix implementation for Os::RawTime
// ======================================================================
#include <Fw/Logger/Logger.hpp>
#include <Fw/Types/Assert.hpp>
#include <Os/Posix/RawTime.hpp>
#include <Os/Posix/error.hpp>
#include <cerrno>

namespace Os {
namespace Posix {
namespace RawTime {

PosixRawTime::Status PosixRawTime::now() {
    PlatformIntType status = clock_gettime(CLOCK_REALTIME, &this->m_handle.m_timespec);
    if (status != 0) {
        return errno_to_rawtime_status(errno);
    }
    return Status::OP_OK;
}

PosixRawTime::Status PosixRawTime::getTimeInterval(const Os::RawTime& other, Fw::TimeInterval& interval) const {
    timespec t1 = this->m_handle.m_timespec;
    timespec t2 = static_cast<PosixRawTimeHandle*>(const_cast<Os::RawTime&>(other).getHandle())->m_timespec;

    // Guarantee t1 is the later time to make the calculation below easier
    if ((t1.tv_sec < t2.tv_sec) or (t1.tv_sec == t2.tv_sec and t1.tv_nsec < t2.tv_nsec)) {
        t1 = static_cast<PosixRawTimeHandle*>(const_cast<Os::RawTime&>(other).getHandle())->m_timespec;
        t2 = this->m_handle.m_timespec;
    }

    // Here we have guaranteed that t1 > t2, so there is no underflow
    U32 sec = static_cast<U32>(t1.tv_sec - t2.tv_sec);
    U32 nanosec = 0;
    if (t1.tv_nsec < t2.tv_nsec) {
        sec -= 1;  // subtract nsec carry to seconds
        nanosec = static_cast<U32>(t1.tv_nsec + (1000000000 - t2.tv_nsec));
    } else {
        nanosec = static_cast<U32>(t1.tv_nsec - t2.tv_nsec);
    }

    interval.set(sec, nanosec / 1000);
    return Status::OP_OK;
}

Fw::SerializeStatus PosixRawTime::serialize(Fw::SerializeBufferBase& buffer) const {
    static_assert(PosixRawTime::SERIALIZED_SIZE >= 2 * sizeof(U32),
                  "PosixRawTime implementation requires at least 2*sizeof(U32) serialization size");
    Fw::SerializeStatus status = buffer.serialize(static_cast<U32>(this->m_handle.m_timespec.tv_sec));
    if (status != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        return status;
    }
    return buffer.serialize(static_cast<U32>(this->m_handle.m_timespec.tv_nsec));
}

Fw::SerializeStatus PosixRawTime::deserialize(Fw::SerializeBufferBase& buffer) {
    static_assert(PosixRawTime::SERIALIZED_SIZE >= 2 * sizeof(U32),
                  "PosixRawTime implementation requires at least 2*sizeof(U32) serialization size");
    U32 sec = 0;
    U32 nsec = 0;
    Fw::SerializeStatus status = buffer.deserialize(sec);
    if (status != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        return status;
    }
    status = buffer.deserialize(nsec);
    if (status != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        return status;
    }
    this->m_handle.m_timespec = {static_cast<time_t>(sec), static_cast<long>(nsec)};
    return Fw::SerializeStatus::FW_SERIALIZE_OK;
}

RawTimeHandle* PosixRawTime::getHandle() {
    return &this->m_handle;
}

}  // namespace RawTime
}  // namespace Posix
}  // namespace Os
