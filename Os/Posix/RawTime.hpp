// ======================================================================
// \title Os/Posix/RawTime.hpp
// \brief Posix definitions for Os::RawTime
// ======================================================================
#ifndef OS_POSIX_RAWTIME_HPP
#define OS_POSIX_RAWTIME_HPP

#include <ctime>
#include <Os/RawTime.hpp>


namespace Os {
namespace Posix {
namespace RawTime {

struct PosixRawTimeHandle : public RawTimeHandle {
    timespec m_timespec = timespec{0, 0};
};

//! \brief Posix implementation of Os::RawTime
//!
//! Posix implementation of `RawTimeInterface` for use as a delegate class handling error-only file operations.
//!
class PosixRawTime : public RawTimeInterface {
  public:

    //! \brief constructor
    PosixRawTime() = default;

    //! \brief destructor
    ~PosixRawTime() override = default;

    //! \brief return the underlying RawTime handle (implementation specific)
    //! \return internal RawTime handle representation
    RawTimeHandle* getHandle() override;

    Status getRawTime() override;
    Status getTimeInterval(const Os::RawTime& other, Fw::TimeInterval& interval) const override;
    Fw::SerializeStatus serialize(Fw::SerializeBufferBase& buffer) const override;  //!< serialize contents
    Fw::SerializeStatus deserialize(Fw::SerializeBufferBase& buffer) override;      //!< deserialize to contents


  private:
    //! Handle for PosixRawTime
    PosixRawTimeHandle m_handle;
};

}  // namespace RawTime
}  // namespace Posix
}  // namespace Os
#endif  // OS_POSIX_RAWTIME_HPP
