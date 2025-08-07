// ======================================================================
// \title RawTimeTester
// \brief Testing implementation of Os::RawTime
// ======================================================================
#ifndef SVC_OSTIME_TEST_UT_RAWTIMETESTER_HPP
#define SVC_OSTIME_TEST_UT_RAWTIMETESTER_HPP

#include "Fw/Time/Time.hpp"
#include "Os/RawTime.hpp"

namespace Svc {

struct RawTimeTesterHandle : public Os::RawTimeHandle {
    Fw::Time t;
};

//! \brief stub implementation of Os::RawTime
//!
//! Stub implementation of `RawTimeInterface`.
//!
class RawTimeTester : public Os::RawTimeInterface {
  public:
    //! \brief constructor
    //!
    RawTimeTester() = default;

    //! \brief destructor
    //!
    ~RawTimeTester() override = default;

    //! \brief return the underlying RawTime handle (implementation specific)
    //! \return internal RawTime handle representation
    Os::RawTimeHandle* getHandle() override { return reinterpret_cast<Os::RawTimeHandle*>(&m_handle); }

    Status now() override {
        m_handle.t = s_now_time;
        return OP_OK;
    }

    Status getTimeInterval(const Os::RawTime& other, Fw::TimeInterval& interval) const override {
        const RawTimeTesterHandle* other_handle =
            reinterpret_cast<const RawTimeTesterHandle*>(const_cast<Os::RawTime&>(other).getHandle());

        Fw::TimeInterval t_start = Fw::TimeInterval(other_handle->t.getSeconds(), other_handle->t.getUSeconds());
        Fw::TimeInterval t_end = Fw::TimeInterval(m_handle.t.getSeconds(), m_handle.t.getUSeconds());

        interval = Fw::TimeInterval::sub(t_start, t_end);
        return OP_OK;
    }

    Fw::SerializeStatus serializeTo(Fw::SerializeBufferBase& buffer) const override {
        return buffer.serializeFrom(m_handle.t);
    }

    Fw::SerializeStatus deserializeFrom(Fw::SerializeBufferBase& buffer) override {
        return buffer.deserializeTo(m_handle.t);
    }

    static void setNowTime(const Fw::Time&& t) { s_now_time = t; }

  private:
    static Fw::Time s_now_time;

    //! Handle for RawTimeTester
    RawTimeTesterHandle m_handle;
};

}  // namespace Svc

#endif  // SVC_OSTIME_TEST_UT_RAWTIMETESTER_HPP
