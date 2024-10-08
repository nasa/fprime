// ======================================================================
// \title Os/Posix/RawTime.hpp
// \brief Posix definitions for Os::RawTime
// ======================================================================
#ifndef OS_POSIX_RAWTIME_HPP
#define OS_POSIX_RAWTIME_HPP
#include <ctime>
#include <Os/RawTime.hpp>

// static constexpr FwSizeType OS_RAWTIME_SERIALIZED_SIZE = sizeof(timespec);

// Questions:
// - U32 for result in getDiffUsec ? Should I use a FwTimePrecision type or something instead?
// - U32 microsec of diff means you can only have ~4000 seconds of difference before overflow ???
// - is the process of using std::chrono for shadow times good ?
// - operations in getDiffUsec are not safe from overflow  - return a status is good enough?
// - How to handle TimerVal constructor for testing... not much of a question
// - Should IntervalTimer be moved to Fw ??? It's not really Os anymore --> No
// - How to handle OS_RAWTIME_SERIALIZED_SIZE ???




namespace Os {
namespace Posix {
namespace RawTime {

struct PosixRawTimeHandle : public RawTimeHandle {
    timespec m_timespec = {0, 0}; // do it the stupidly verbose way
};

//! \brief Posix implementation of Os::RawTime
//!
//! Posix implementation of `RawTimeInterface` for use as a delegate class handling error-only file operations.
//!
class PosixRawTime : public RawTimeInterface {
  public:
    // static const FwSizeType SERIALIZED_SIZE = RawTimeInterface::SERIALIZED_SIZE;

    //! \brief constructor
    PosixRawTime();

    //! \brief destructor
    ~PosixRawTime() override;

    //! \brief return the underlying RawTime handle (implementation specific)
    //! \return internal RawTime handle representation
    RawTimeHandle* getHandle() override;

    Status getRawTime() override;
    Status getTimeInterval(const RawTimeHandle& other, Fw::TimeInterval& interval) const override;
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
