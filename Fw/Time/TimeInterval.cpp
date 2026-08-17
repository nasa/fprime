#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Time/TimeInterval.hpp>

namespace Fw {

// Microseconds-per-second invariant shared by carry logic and range checks
constexpr U32 MICROSECONDS_PER_SECOND = 1000000;
TimeInterval::TimeInterval(const TimeInterval& other) : Serializable() {
    this->m_val = other.m_val;
}

TimeInterval::TimeInterval(U32 seconds, U32 useconds) : Serializable() {
    this->set(seconds, useconds);
}

TimeInterval::TimeInterval(const Time& start, const Time& end)
    : TimeInterval(TimeInterval::sub(TimeInterval(end.getSeconds(), end.getUSeconds()),
                                     TimeInterval(start.getSeconds(), start.getUSeconds()))) {}

void TimeInterval::set(U32 seconds, U32 useconds) {
    // Assert microseconds portion is less than 10^6
    FW_ASSERT(useconds < MICROSECONDS_PER_SECOND, static_cast<FwAssertArgType>(useconds));
    this->m_val.set(seconds, useconds);
}

TimeInterval& TimeInterval::operator=(const TimeInterval& other) {
    if (this != &other) {
        this->m_val = other.m_val;
    }
    return *this;
}

bool TimeInterval::operator==(const TimeInterval& other) const {
    return (TimeInterval::compare(*this, other) == EQ);
}

bool TimeInterval::operator!=(const TimeInterval& other) const {
    return (TimeInterval::compare(*this, other) != EQ);
}

bool TimeInterval::operator>(const TimeInterval& other) const {
    return (TimeInterval::compare(*this, other) == GT);
}

bool TimeInterval::operator<(const TimeInterval& other) const {
    return (TimeInterval::compare(*this, other) == LT);
}

bool TimeInterval::operator>=(const TimeInterval& other) const {
    TimeInterval::Comparison c = TimeInterval::compare(*this, other);
    return ((GT == c) or (EQ == c));
}

bool TimeInterval::operator<=(const TimeInterval& other) const {
    TimeInterval::Comparison c = TimeInterval::compare(*this, other);
    return ((LT == c) or (EQ == c));
}

TimeIntervalValue TimeInterval::asTimeIntervalValue() const {
    return this->m_val;
}

SerializeStatus TimeInterval::serializeTo(SerialBufferBase& buffer, Fw::Endianness mode) const {
    // Use TimeIntervalValue's built-in serialization
    return this->m_val.serializeTo(buffer, mode);
}

SerializeStatus TimeInterval::deserializeFrom(SerialBufferBase& buffer, Fw::Endianness mode) {
    // Deserialize to a temporary and validate the microseconds invariant before committing
    TimeIntervalValue value;
    const SerializeStatus status = value.deserializeFrom(buffer, mode);
    if (status != FW_SERIALIZE_OK) {
        return status;
    }
    if (value.get_useconds() >= MICROSECONDS_PER_SECOND) {
        return FW_DESERIALIZE_FORMAT_ERROR;
    }
    this->m_val = value;
    return FW_SERIALIZE_OK;
}

U32 TimeInterval::getSeconds() const {
    return this->m_val.get_seconds();
}

U32 TimeInterval::getUSeconds() const {
    return this->m_val.get_useconds();
}

TimeInterval::Comparison TimeInterval ::compare(const TimeInterval& time1, const TimeInterval& time2) {
    const U32 s1 = time1.getSeconds();
    const U32 s2 = time2.getSeconds();
    const U32 us1 = time1.getUSeconds();
    const U32 us2 = time2.getUSeconds();

    if (s1 < s2) {
        return LT;
    } else if (s1 > s2) {
        return GT;
    } else if (us1 < us2) {
        return LT;
    } else if (us1 > us2) {
        return GT;
    } else {
        return EQ;
    }
}

TimeInterval TimeInterval ::add(const TimeInterval& a, const TimeInterval& b) {
    U32 seconds = a.getSeconds() + b.getSeconds();
    U32 uSeconds = a.getUSeconds() + b.getUSeconds();
    FW_ASSERT(uSeconds < 2 * MICROSECONDS_PER_SECOND - 1);
    if (uSeconds >= MICROSECONDS_PER_SECOND) {
        ++seconds;
        uSeconds -= MICROSECONDS_PER_SECOND;
    }
    TimeInterval c(seconds, uSeconds);
    return c;
}

TimeInterval TimeInterval ::sub(const TimeInterval& t1,  //!< TimeInterval t1
                                const TimeInterval& t2   //!< TimeInterval t2
) {
    const TimeInterval& minuend = (t1 > t2) ? t1 : t2;
    const TimeInterval& subtrahend = (t1 > t2) ? t2 : t1;

    U32 seconds = minuend.getSeconds() - subtrahend.getSeconds();
    U32 uSeconds;
    if (subtrahend.getUSeconds() > minuend.getUSeconds()) {
        seconds--;
        uSeconds = minuend.getUSeconds() + MICROSECONDS_PER_SECOND - subtrahend.getUSeconds();
    } else {
        uSeconds = minuend.getUSeconds() - subtrahend.getUSeconds();
    }
    // Microseconds portion must be normalized to less than 10^6
    FW_ASSERT(uSeconds < MICROSECONDS_PER_SECOND, static_cast<FwAssertArgType>(uSeconds));
    return TimeInterval(seconds, static_cast<U32>(uSeconds));
}

void TimeInterval::add(U32 seconds, U32 useconds) {
    U32 newSeconds = this->m_val.get_seconds() + seconds;
    U32 newUSeconds = this->m_val.get_useconds() + useconds;
    FW_ASSERT(newUSeconds < 2 * MICROSECONDS_PER_SECOND - 1, static_cast<FwAssertArgType>(newUSeconds));
    if (newUSeconds >= MICROSECONDS_PER_SECOND) {
        newSeconds += 1;
        newUSeconds -= MICROSECONDS_PER_SECOND;
    }
    // Assert microseconds portion is less than 10^6
    FW_ASSERT(newUSeconds < MICROSECONDS_PER_SECOND, static_cast<FwAssertArgType>(newUSeconds));
    this->m_val.set(newSeconds, newUSeconds);
}

#ifdef BUILD_UT
std::ostream& operator<<(std::ostream& os, const TimeInterval& val) {
    os << "(" << val.getSeconds() << "s," << val.getUSeconds() << "us)";
    return os;
}
#endif

}  // namespace Fw
