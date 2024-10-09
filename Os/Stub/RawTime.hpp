// ======================================================================
// \title Os/Stub/RawTime.hpp
// \brief stub definitions for Os::RawTime
// ======================================================================
#ifndef OS_STUB_RAWTIME_HPP
#define OS_STUB_RAWTIME_HPP

#include "Os/RawTime.hpp"

namespace Os {
namespace Stub {
namespace RawTime {

struct StubRawTimeHandle : public RawTimeHandle {};

//! \brief stub implementation of Os::RawTime
//!
//! Stub implementation of `RawTimeInterface`.
//!
class StubRawTime : public RawTimeInterface {
  public:
    //! \brief constructor
    //!
    StubRawTime() = default;

    //! \brief destructor
    //!
    ~StubRawTime() override = default;

    //! \brief return the underlying RawTime handle (implementation specific)
    //! \return internal RawTime handle representation
    RawTimeHandle* getHandle() override;

    Status getRawTime() override;
    Status getTimeInterval(const Os::RawTime& other, Fw::TimeInterval& interval) const override;  //!<  docs
    Fw::SerializeStatus serialize(Fw::SerializeBufferBase& buffer) const override;  //!< serialize contents
    Fw::SerializeStatus deserialize(Fw::SerializeBufferBase& buffer) override;      //!< deserialize to contents

  private:
    //! Handle for StubRawTime
    StubRawTimeHandle m_handle;
};

} // namespace RawTime
} // namespace Stub
} // namespace Os
#endif // OS_STUB_RAWTIME_HPP
