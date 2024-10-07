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
    Fw::Logger::log("Getting diff usec!!\n");
    Fw::TimeInterval interval;
    Status status = this->getTimeInterval(other, interval);
    if (status != Status::OP_OK) {
        return status;
    }

    // Check overflows in computation
    U32 seconds = interval.getSeconds();
    U32 useconds = interval.getUSeconds();
    if (seconds > (std::numeric_limits<U32>::max() / 1000000)) {
        return Status::OP_OVERFLOW;
    }
    U32 secToUsec = seconds * 1000000;
    if (secToUsec > (std::numeric_limits<U32>::max() - useconds)) {
        return Status::OP_OVERFLOW;
    }
    // No overflow, we can safely add values to get total microseconds
    result = secToUsec + useconds;
    return status;
}

PosixRawTime::Status PosixRawTime::getTimeInterval(const RawTimeHandle& other, Fw::TimeInterval& interval) const {
    timespec t1 = this->m_handle.m_timespec;
    timespec t2 = const_cast<const PosixRawTimeHandle*>(static_cast<const PosixRawTimeHandle*>(&other))->m_timespec;

    // Guarantee t1 is the later time to make the calculation below easier
    if ((t1.tv_sec < t2.tv_sec) or (t1.tv_sec == t2.tv_sec and t1.tv_nsec < t2.tv_nsec)) {
        t2 = this->m_handle.m_timespec;
        t1 = const_cast<const PosixRawTimeHandle*>(static_cast<const PosixRawTimeHandle*>(&other))->m_timespec;
    } 

    // Here we have guaranteed that t1 > t2, so there is no underflow
    U32 sec = static_cast<U32>(t1.tv_sec - t2.tv_sec);
    U32 nanosec = 0;
    if (t1.tv_nsec < t2.tv_nsec) {
        sec -= 1; // subtract nsec carry to seconds
        nanosec = static_cast<U32>(t1.tv_nsec + (1000000000 - t2.tv_nsec));
    } else {
        nanosec = static_cast<U32>(t1.tv_nsec - t2.tv_nsec);
    }

    interval.set(sec, nanosec / 1000);
    return Status::OP_OK;
}


RawTimeHandle* PosixRawTime::getHandle() {
    return &this->m_handle;
}

Fw::SerializeStatus PosixRawTime::serialize(Fw::SerializeBufferBase& buffer) const {
    Fw::Logger::log("Serializing raw time\n");
    FW_ASSERT(PosixRawTime::SERIALIZED_SIZE >= 2 * sizeof(U32));
    buffer.serialize(static_cast<U32>(this->m_handle.m_timespec.tv_sec));
    buffer.serialize(static_cast<U32>(this->m_handle.m_timespec.tv_nsec));
    return Fw::SerializeStatus::FW_SERIALIZE_OK;
}

Fw::SerializeStatus PosixRawTime::deserialize(Fw::SerializeBufferBase& buffer) {
    Fw::Logger::log("Deserializing raw time\n");
    FW_ASSERT(PosixRawTime::SERIALIZED_SIZE >= 2 * sizeof(U32));
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
