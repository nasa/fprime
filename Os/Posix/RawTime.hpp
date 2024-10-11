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
    timespec m_timespec = {0, 0};
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

    // ------------------------------------------------------------
    // Implementation-specific RawTime overrides
    // ------------------------------------------------------------
    //! \brief Get the current time.
    //!
    //! This function retrieves the current time and stores it in the RawTime object.
    //! Each implementation should define its RawTimeHandle type for storing the time.
    //!
    //! \return Status indicating the result of the operation.
    Status now() override;

    //! \brief Calculate the time interval between this and another raw time.
    //!
    //! This function calculates the time interval between the current raw time and another
    //! specified raw time. The result is stored in the provided (output) interval object.
    //!
    //! \param other The other RawTimeHandle to compare against.
    //! \param interval Output parameter to store the calculated time interval.
    //! \return Status indicating the result of the operation.
    Status getTimeInterval(const Os::RawTime& other, Fw::TimeInterval& interval) const override;

    //! \brief Serialize the contents of the RawTimeInterface object into a buffer.
    //!
    //! This function serializes the contents of the RawTimeInterface object into the provided
    //! buffer. 
    //!
    //! \note The serialization must fit within `FW_RAW_TIME_SERIALIZATION_MAX_SIZE` bytes. This value is
    //! defined in FpConfig.h. For example, Posix systems use a pair of U32 (sec, nanosec) and can therefore
    //! serialize in 8 bytes. Should an OSAL implementation require more than this, the project must increase 
    //! that value in its config/ folder.
    //!
    //! \param buffer The buffer to serialize the contents into.
    //! \return Fw::SerializeStatus indicating the result of the serialization.
    Fw::SerializeStatus serialize(Fw::SerializeBufferBase& buffer) const override;

    //! \brief Deserialize the contents of the RawTimeInterface object from a buffer.
    //!
    //! This function deserializes the contents of the RawTimeInterface object from the provided
    //! buffer.
    //!
    //! \note The serialization must fit within `FW_RAW_TIME_SERIALIZATION_MAX_SIZE` bytes. This value is
    //! defined in FpConfig.h. For example, Posix systems use a pair of U32 (sec, nanosec) and can therefore
    //! serialize in 8 bytes. Should an OSAL implementation require more than this, the project must increase 
    //! that value in its config/ folder.
    //!
    //! \param buffer The buffer to deserialize the contents from.
    //! \return Fw::SerializeStatus indicating the result of the deserialization.
    Fw::SerializeStatus deserialize(Fw::SerializeBufferBase& buffer) override;


  private:
    //! Handle for PosixRawTime
    PosixRawTimeHandle m_handle;
};

}  // namespace RawTime
}  // namespace Posix
}  // namespace Os
#endif  // OS_POSIX_RAWTIME_HPP
