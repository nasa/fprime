#include <Fw/Time/TimeInterval.hpp>
#include <FpConfig.hpp>

namespace Fw {
    TimeInterval::TimeInterval(const TimeInterval& other) : Serializable() {
        this->set(other.m_seconds,other.m_useconds);
    }

    TimeInterval::TimeInterval(U32 seconds, U32 useconds) : Serializable() {
        this->set(seconds,useconds);
    }

    void TimeInterval::set(U32 seconds, U32 useconds) {
        this->m_seconds = seconds;
        this->m_useconds = useconds;
    }

    TimeInterval& TimeInterval::operator=(const TimeInterval& other) {
        this->m_useconds = other.m_useconds;
        this->m_seconds = other.m_seconds;

        return *this;
    }

    bool TimeInterval::operator==(const TimeInterval& other) const {
        return (TimeInterval::compare(*this,other) == EQ);
    }

    bool TimeInterval::operator!=(const TimeInterval& other) const {
        return (TimeInterval::compare(*this,other) != EQ);
    }

    bool TimeInterval::operator>(const TimeInterval& other) const {
        return (TimeInterval::compare(*this,other) == GT);
    }

    bool TimeInterval::operator<(const TimeInterval& other) const {
        return (TimeInterval::compare(*this,other) == LT);
    }

    bool TimeInterval::operator>=(const TimeInterval& other) const {
        TimeInterval::Comparison c = TimeInterval::compare(*this,other);
        return ((GT == c) or (EQ == c));
    }

    bool TimeInterval::operator<=(const TimeInterval& other) const {
        TimeInterval::Comparison c = TimeInterval::compare(*this,other);
        return ((LT == c) or (EQ == c));
    }

    SerializeStatus TimeInterval::serialize(SerializeBufferBase& buffer) const {
        // serialize members
        SerializeStatus stat = Fw::FW_SERIALIZE_OK;
        stat = buffer.serialize(this->m_seconds);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }
        return buffer.serialize(this->m_useconds);
    }

    SerializeStatus TimeInterval::deserialize(SerializeBufferBase& buffer) {

        SerializeStatus stat = Fw::FW_SERIALIZE_OK;
        stat = buffer.deserialize(this->m_seconds);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

        return buffer.deserialize(this->m_useconds);
    }

    U32 TimeInterval::getSeconds() const {
        return this->m_seconds;
    }

    U32 TimeInterval::getUSeconds() const {
        return this->m_useconds;
    }

    TimeInterval::Comparison TimeInterval ::
      compare(
          const TimeInterval &time1,
          const TimeInterval &time2
      )
    {
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

    TimeInterval TimeInterval ::
      add(
        const TimeInterval& a,
        const TimeInterval& b
      )
    {
      U32 seconds = a.getSeconds() + b.getSeconds();
      U32 uSeconds = a.getUSeconds() + b.getUSeconds();
      FW_ASSERT(uSeconds < 1999999);
      if (uSeconds >= 1000000) {
        ++seconds;
        uSeconds -= 1000000;
      }
      TimeInterval c(seconds,uSeconds);
      return c;
    }

    TimeInterval TimeInterval ::
      sub(
        const TimeInterval& t1, //!< TimeInterval t1
        const TimeInterval& t2 //!< TimeInterval t2
    )
    {
      const TimeInterval& minuend = (t1 > t2) ? t1 : t2;
      const TimeInterval& subtrahend = (t1 > t2) ? t2 : t1;

      U32 seconds = minuend.getSeconds() - subtrahend.getSeconds();
      U32 uSeconds;
      if (subtrahend.getUSeconds() > minuend.getUSeconds()) {
          seconds--;
          uSeconds = minuend.getUSeconds() + 1000000 - subtrahend.getUSeconds();
      } else {
          uSeconds = minuend.getUSeconds() - subtrahend.getUSeconds();
      }
      return TimeInterval(seconds, static_cast<U32>(uSeconds));
    }

    void TimeInterval::add(U32 seconds, U32 useconds) {
        this->m_seconds += seconds;
        this->m_useconds += useconds;
        FW_ASSERT(this->m_useconds < 1999999, static_cast<FwAssertArgType>(this->m_useconds));
        if (this->m_useconds >= 1000000) {
          this->m_seconds += 1;
          this->m_useconds -= 1000000;
        }
    }

#ifdef BUILD_UT
    std::ostream& operator<<(std::ostream& os, const TimeInterval& val) {

        os << "(" << val.getSeconds() << "s," << val.getUSeconds() << "us)";
        return os;
    }
#endif

}
