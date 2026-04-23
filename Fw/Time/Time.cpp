#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Time/Time.hpp>

namespace Fw {
const Time ZERO_TIME = Time();

Time::Time() : m_val() {
    m_val.set_timeBase(TimeBase::TB_NONE);
    m_val.set_timeContext(0);
    m_val.set_seconds(0);
    m_val.set_useconds(0);
}

Time::~Time() {}

Time::Time(const Time& other) : Serializable() {
    this->set(other.m_val.get_timeBase(), other.m_val.get_timeContext(), other.m_val.get_seconds(),
              other.m_val.get_useconds());
}

Time::Time(U32 seconds, U32 useconds) {
    this->set(TimeBase::TB_NONE, 0, seconds, useconds);
}

Time::Time(TimeBase timeBase, U32 seconds, U32 useconds) {
    this->set(timeBase, 0, seconds, useconds);
}

void Time::set(U32 seconds, U32 useconds) {
    this->set(this->m_val.get_timeBase(), this->m_val.get_timeContext(), seconds, useconds);
}

void Time::set(TimeBase timeBase, U32 seconds, U32 useconds) {
    this->set(timeBase, this->m_val.get_timeContext(), seconds, useconds);
}

Time::Time(TimeBase timeBase, FwTimeContextStoreType context, U32 seconds, U32 useconds) {
    this->set(timeBase, context, seconds, useconds);
}

void Time::set(TimeBase timeBase, FwTimeContextStoreType context, U32 seconds, U32 useconds) {
    // Assert microseconds portion is less than 10^6
    FW_ASSERT(useconds < 1000000, static_cast<FwAssertArgType>(useconds));
    this->m_val.set(timeBase, context, seconds, useconds);
}

Fw::Time::Time(F64 seconds) {
    this->set(seconds);
}

void Fw::Time::set(F64 seconds) {
    U32 parsedSeconds = this->parseSeconds(seconds);
    U32 parsedUseconds = this->parseUSeconds(seconds);
    this->set(parsedSeconds, parsedUseconds);
}

Time& Time::operator=(const Time& other) {
    if (this != &other) {
        this->m_val = other.m_val;
    }
    return *this;
}

Time& Fw::Time::operator=(F64 seconds) {
    this->set(seconds);
    return *this;
}

Time& Fw::Time::operator+=(F64 seconds) {
    this->add(seconds);
    return *this;
}

bool Time::operator==(const Time& other) const {
    return (Time::compare(*this, other) == TimeComparison::EQ);
}

bool Time::operator!=(const Time& other) const {
    return (Time::compare(*this, other) != TimeComparison::EQ);
}

bool Time::operator>(const Time& other) const {
    return (Time::compare(*this, other) == TimeComparison::GT);
}

bool Time::operator<(const Time& other) const {
    return (Time::compare(*this, other) == TimeComparison::LT);
}

bool Time::operator>=(const Time& other) const {
    TimeComparison c = Time::compare(*this, other);
    return ((TimeComparison::GT == c) or (TimeComparison::EQ == c));
}

bool Time::operator<=(const Time& other) const {
    TimeComparison c = Time::compare(*this, other);
    return ((TimeComparison::LT == c) or (TimeComparison::EQ == c));
}

Fw::Time::operator F64() const {
    const U32 seconds = this->m_val.get_seconds();
    const U32 useconds = this->m_val.get_useconds();
    return static_cast<F64>(seconds) + (static_cast<F64>(useconds) / 1000000.0);
}

TimeValue Time::asTimeValue() const {
    return this->m_val;
}

SerializeStatus Time::serializeTo(SerialBufferBase& buffer, Fw::Endianness mode) const {
    return this->m_val.serializeTo(buffer, mode);
}

SerializeStatus Time::deserializeFrom(SerialBufferBase& buffer, Fw::Endianness mode) {
    return this->m_val.deserializeFrom(buffer, mode);
}

U32 Time::getSeconds() const {
    return this->m_val.get_seconds();
}

U32 Time::getUSeconds() const {
    return this->m_val.get_useconds();
}

TimeBase Time::getTimeBase() const {
    return this->m_val.get_timeBase();
}

FwTimeContextStoreType Time::getContext() const {
    return this->m_val.get_timeContext();
}

Time Time ::zero(TimeBase timeBase) {
    Time time(timeBase, 0, 0, 0);
    return time;
}

TimeComparison Time ::compare(const Time& time1, const Time& time2) {
    if (time1.getTimeBase() != time2.getTimeBase()) {
        return TimeComparison::INCOMPARABLE;
    }

    // Do not compare time context

    const U32 s1 = time1.getSeconds();
    const U32 s2 = time2.getSeconds();
    const U32 us1 = time1.getUSeconds();
    const U32 us2 = time2.getUSeconds();

    if (s1 < s2) {
        return TimeComparison::LT;
    } else if (s1 > s2) {
        return TimeComparison::GT;
    } else if (us1 < us2) {
        return TimeComparison::LT;
    } else if (us1 > us2) {
        return TimeComparison::GT;
    } else {
        return TimeComparison::EQ;
    }
}

Time Time ::add(const Time& a, const Time& b) {
    FW_ASSERT(a.getTimeBase() == b.getTimeBase(), static_cast<FwAssertArgType>(a.getTimeBase()),
              static_cast<FwAssertArgType>(b.getTimeBase()));
    // Do not assert on time context match

    U32 seconds = a.getSeconds() + b.getSeconds();
    U32 uSeconds = a.getUSeconds() + b.getUSeconds();
    FW_ASSERT(uSeconds < 1999999);
    if (uSeconds >= 1000000) {
        ++seconds;
        uSeconds -= 1000000;
    }

    // Return a time context of 0 if they do not match
    FwTimeContextStoreType context = a.getContext();
    if (a.getContext() != b.getContext()) {
        context = 0;
    }

    Time c(a.getTimeBase(), context, seconds, uSeconds);
    return c;
}

Time Time ::sub(const Time& minuend,    //!< Time minuend
                const Time& subtrahend  //!< Time subtrahend
) {
    FW_ASSERT(minuend.getTimeBase() == subtrahend.getTimeBase(), static_cast<FwAssertArgType>(minuend.getTimeBase()),
              static_cast<FwAssertArgType>(subtrahend.getTimeBase()));
    // Do not assert on time context match
    // Assert minuend is greater than subtrahend
    FW_ASSERT(minuend >= subtrahend);

    U32 seconds = minuend.getSeconds() - subtrahend.getSeconds();
    U32 uSeconds;
    if (subtrahend.getUSeconds() > minuend.getUSeconds()) {
        seconds--;
        uSeconds = minuend.getUSeconds() + 1000000 - subtrahend.getUSeconds();
    } else {
        uSeconds = minuend.getUSeconds() - subtrahend.getUSeconds();
    }

    // Return a time context of 0 if they do not match
    FwTimeContextStoreType context = minuend.getContext();
    if (minuend.getContext() != subtrahend.getContext()) {
        context = 0;
    }

    return Time(minuend.getTimeBase(), context, seconds, static_cast<U32>(uSeconds));
}

U32 Fw::Time::parseSeconds(F64 seconds) {
    // Assert negative value
    FW_ASSERT(seconds >= static_cast<F64>(0.0));
    return static_cast<U32>(seconds);
}

U32 Fw::Time::parseUSeconds(F64 seconds) {
    // Assert negative value
    FW_ASSERT(seconds >= static_cast<F64>(0.0));
    U32 parsedSeconds = static_cast<U32>(seconds);
    const F64 fractionalPart = seconds - static_cast<F64>(parsedSeconds);
    // Add 0.5 to round to nearest microsecond (e.g, 999999.9999 = 1000000)
    U32 parsedUSeconds = static_cast<U32>(fractionalPart * static_cast<F64>(1000000.0) + static_cast<F64>(0.5));
    return parsedUSeconds;
}

void Time::add(U32 seconds, U32 useconds) {
    U32 newSeconds = this->m_val.get_seconds() + seconds;
    U32 newUSeconds = this->m_val.get_useconds() + useconds;
    FW_ASSERT(newUSeconds < 1999999, static_cast<FwAssertArgType>(newUSeconds));
    if (newUSeconds >= 1000000) {
        newSeconds += 1;
        newUSeconds -= 1000000;
    }
    this->set(newSeconds, newUSeconds);
}

void Time::add(F64 seconds) {
    U32 parsedSeconds = this->parseSeconds(seconds);
    U32 parsedUseconds = this->parseUSeconds(seconds);
    // Add parsed seconds and useconds
    this->add(parsedSeconds, parsedUseconds);
}

void Time::setTimeBase(TimeBase timeBase) {
    this->m_val.set_timeBase(timeBase);
}

void Time::setTimeContext(FwTimeContextStoreType context) {
    this->m_val.set_timeContext(context);
}

#ifdef BUILD_UT
std::ostream& operator<<(std::ostream& os, const Time& val) {
    os << "(" << val.getTimeBase() << "," << val.getUSeconds() << "," << val.getSeconds() << ")";
    return os;
}
#endif

}  // namespace Fw
