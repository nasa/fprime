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
    this->m_val.set(timeBase, context, seconds, useconds);
}

Time& Time::operator=(const Time& other) {
    if (this != &other) {
        this->m_val = other.m_val;
    }
    return *this;
}

bool Time::operator==(const Time& other) const {
    return (Time::compare(*this, other) == EQ);
}

bool Time::operator!=(const Time& other) const {
    return (Time::compare(*this, other) != EQ);
}

bool Time::operator>(const Time& other) const {
    return (Time::compare(*this, other) == GT);
}

bool Time::operator<(const Time& other) const {
    return (Time::compare(*this, other) == LT);
}

bool Time::operator>=(const Time& other) const {
    Time::Comparison c = Time::compare(*this, other);
    return ((GT == c) or (EQ == c));
}

bool Time::operator<=(const Time& other) const {
    Time::Comparison c = Time::compare(*this, other);
    return ((LT == c) or (EQ == c));
}

SerializeStatus Time::serializeTo(SerializeBufferBase& buffer) const {
    return this->m_val.serializeTo(buffer);
}

SerializeStatus Time::deserializeFrom(SerializeBufferBase& buffer) {
    return this->m_val.deserializeFrom(buffer);
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

Time::Comparison Time ::compare(const Time& time1, const Time& time2) {
    if (time1.getTimeBase() != time2.getTimeBase()) {
        return INCOMPARABLE;
    }

    // Do not compare time context

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
