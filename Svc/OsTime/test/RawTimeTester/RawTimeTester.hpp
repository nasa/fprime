// ======================================================================
// \title RawTimeTester
// \brief Testing implementation of Os::RawTime
// ======================================================================
#ifndef SVC_OSTIME_TEST_UT_RAWTIMETESTER_HPP
#define SVC_OSTIME_TEST_UT_RAWTIMETESTER_HPP

#include "Fw/Time/Time.hpp"
#include "Fw/Types/SerialBuffer.hpp"
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
        if (s_now_status != OP_OK) {
            return s_now_status;
        }
        m_handle.t = s_now_time;
        return OP_OK;
    }

    Status getTimeInterval(const Os::RawTime& other, Fw::TimeInterval& interval) const override {
        // Use serialization to access other's time without assuming internal structure.
        // Works with both link-time and compile-time implementations.
        // NOTE: Assumes serialization format (seconds: U32, useconds: U32) matching
        // standard Posix/Stub implementations. Platform implementations must match.
        U8 buf_data[Os::RawTimeInterface::SERIALIZED_SIZE];
        Fw::SerialBuffer buf(buf_data, sizeof(buf_data));
        other.serializeTo(buf);
        buf.resetDeser();
        U32 other_seconds = 0;
        U32 other_useconds = 0;
        buf.deserializeTo(other_seconds);
        buf.deserializeTo(other_useconds);
        Fw::TimeInterval t_start(other_seconds, other_useconds);
        Fw::TimeInterval t_end(m_handle.t.getSeconds(), m_handle.t.getUSeconds());
        interval = Fw::TimeInterval::sub(t_start, t_end);
        return OP_OK;
    }

    Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer,
                                    Fw::Endianness mode = Fw::Endianness::BIG) const override {
        // Standard serialization format: (seconds: U32, useconds: U32)
        // This format must match the actual RawTime implementations (Posix, Stub, etc.)
        Fw::SerializeStatus status = buffer.serializeFrom(m_handle.t.getSeconds(), mode);
        if (status != Fw::FW_SERIALIZE_OK) {
            return status;
        }
        return buffer.serializeFrom(m_handle.t.getUSeconds(), mode);
    }

    Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer,
                                        Fw::Endianness mode = Fw::Endianness::BIG) override {
        // Standard deserialization format: (seconds: U32, useconds: U32)
        // This format must match the actual RawTime implementations (Posix, Stub, etc.)
        U32 seconds = 0;
        U32 useconds = 0;
        Fw::SerializeStatus status = buffer.deserializeTo(seconds, mode);
        if (status != Fw::FW_SERIALIZE_OK) {
            return status;
        }
        status = buffer.deserializeTo(useconds, mode);
        if (status != Fw::FW_SERIALIZE_OK) {
            return status;
        }
        m_handle.t.set(seconds, useconds);
        return status;
    }

    static void setNowTime(const Fw::Time&& t) { s_now_time = t; }

    static void setNowStatus(Status status) { s_now_status = status; }

  private:
    static Fw::Time s_now_time;
    static Status s_now_status;

    //! Handle for RawTimeTester
    RawTimeTesterHandle m_handle;
};

}  // namespace Svc

#endif  // SVC_OSTIME_TEST_UT_RAWTIMETESTER_HPP
