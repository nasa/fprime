// ======================================================================
// \title Os/Posix/RawTime.cpp
// \brief Posix implementation for Os::RawTime
// ======================================================================
#include <Os/Posix/RawTime.hpp>
#include <Os/Posix/error.hpp>
#include <Fw/Types/Assert.hpp>
#include <cerrno>
#include <Fw/Logger/Logger.hpp>

namespace Os {
namespace Posix {
namespace RawTime {

PosixRawTime::PosixRawTime() : Os::RawTimeInterface(), m_handle() {

}

PosixRawTime::~PosixRawTime() {

}

PosixRawTime::Status PosixRawTime::getRawTime() {
    Fw::Logger::log("Getting raw time\n");
    PlatformIntType status = clock_gettime(CLOCK_REALTIME, &this->m_handle.m_timespec);
    FW_ASSERT(status == 0, errno); // TODO: error handling
    return Status::OP_OK;
}

PosixRawTime::Status PosixRawTime::getDiffUsec(const RawTimeHandle& other, U32& result) const {
    const timespec t1 = this->m_handle.m_timespec;
    const timespec t2 = const_cast<const PosixRawTimeHandle*>(static_cast<const PosixRawTimeHandle*>(&other))->m_timespec;
    U32 sec = static_cast<U32>(t1.tv_sec - t2.tv_sec);
    U32 nanosec = 0;
    // TODO: better implementation - is U32 ok?
    if (t1.tv_nsec < t2.tv_nsec) {
        sec -= 1; // subtract nsec carry to seconds
        nanosec = static_cast<U32>(t1.tv_nsec + (1000000000 - t2.tv_nsec));
    } else {
        nanosec = static_cast<U32>(t1.tv_nsec - t2.tv_nsec);
    }
    // TODO: return status if overflow ??
    result = (sec * 1000000) + (nanosec / 1000);
    return Status::OP_OK;
}

RawTimeHandle* PosixRawTime::getHandle() {
    return &this->m_handle;
}

Fw::SerializeStatus PosixRawTime::serialize(Fw::SerializeBufferBase& buffer) const {
    Fw::Logger::log("Serializing raw time\n");

    buffer.serialize(static_cast<U32>(this->m_handle.m_timespec.tv_sec));
    buffer.serialize(static_cast<U32>(this->m_handle.m_timespec.tv_nsec));
    return Fw::SerializeStatus::FW_SERIALIZE_OK;
}

Fw::SerializeStatus PosixRawTime::deserialize(Fw::SerializeBufferBase& buffer) {
    Fw::Logger::log("Deserializing raw time\n");

    U32 sec = 0;
    U32 nsec = 0;
    buffer.deserialize(sec);
    buffer.deserialize(nsec);
    this->m_handle.m_timespec = {sec, nsec};
    return Fw::SerializeStatus::FW_SERIALIZE_OK;
}

}  // namespace RawTime
}  // namespace Posix
}  // namespace Os
