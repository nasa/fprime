// ======================================================================
// \title Os/DelegateRawTime.hpp
// \brief Define the Os::DelegateRawTime class
// ======================================================================
#ifndef OS_DELEGATERAWTIME_HPP_
#define OS_DELEGATERAWTIME_HPP_

#include "Os/RawTimeInterface.hpp"

namespace Os {

class DelegateRawTime final : public RawTimeInterface {
  public:
    DelegateRawTime();  //!<  Default constructor

    //! \brief Constructor with timer source selection
    //! \param source Timer source to use (defaults to RAWTIME_DEFAULT)
    explicit DelegateRawTime(RawTimeSource source);

    ~DelegateRawTime() final;  //!<  Destructor

    //! \brief copy constructor that copies the internal representation
    DelegateRawTime(const DelegateRawTime& other);

    //! \brief assignment operator that copies the internal representation
    DelegateRawTime& operator=(const DelegateRawTime& other);

    //! \brief return the underlying RawTime handle (implementation specific)
    //! \return internal RawTime handle representation
    RawTimeHandle* getHandle() override;

    // ------------------------------------------------------------
    // Implementation-specific RawTime member functions
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
    //! \param mode Endianness to use when serializing to buffer.
    //! \return Fw::SerializeStatus indicating the result of the serialization.
    Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer,
                                    Fw::Endianness mode = Fw::Endianness::BIG) const override;

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
    //! \param mode Endianness to use when deserializing from the buffer.
    //! \return Fw::SerializeStatus indicating the result of the deserialization.
    Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer,
                                        Fw::Endianness mode = Fw::Endianness::BIG) override;

    //! \brief Calculate the difference in microseconds between two RawTime objects.
    //!
    //! This function calculates the difference in microseconds between the current RawTime object
    //! and another RawTime object provided as a parameter.
    //!
    //! \warning This function will return Status::OP_OVERFLOW if the time difference is too large to fit in a U32.
    //! \warning This means the largest time difference that can be measured is 2^32 microseconds (about 71 minutes).
    //! \warning Users should prefer getTimeInterval() for larger intervals.
    //!
    //! \param other The other RawTime object to compare against.
    //! \param result A reference to a U32 variable where the result will be stored.
    //! \return Status indicating the result of the operation.
    Status getDiffUsec(const RawTime& other, U32& result) const override;

    // ------------------------------------------------------------
    // Common functions built on top of OS-specific functions
    // ------------------------------------------------------------

    //! \brief Compare whether two RawTime objects are the same (i.e. refer to the same microsecond)
    bool operator==(const RawTime& other) const override;

    //! \brief Get the timer source used by this RawTime instance
    //! \return The RawTimeSource value configured for this instance
    RawTimeSource getSource() const;

  private:
    // This section is used to store the implementation-defined RawTime handle. To Os::RawTime and fprime, this type is
    // opaque and thus normal allocation cannot be done. Instead, we allow the implementor to store the handle in
    // the byte-array here and set `m_handle_storage` to that address for storage.
    //
    alignas(FW_HANDLE_ALIGNMENT) RawTimeHandleStorage m_handle_storage;  //!< RawTime handle storage
    RawTimeInterface& m_delegate;                                        //!< Delegate for the real implementation
    RawTimeSource m_source = RAWTIME_DEFAULT;                            //!< Timer source selection
};
}  // namespace Os

#endif  // OS_DELEGATERAWTIME_HPP_
