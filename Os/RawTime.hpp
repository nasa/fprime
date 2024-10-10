// ======================================================================
// \title Os/RawTime.hpp
// \brief common definitions for Os::RawTime
// ======================================================================
#ifndef OS_RAWTIME_HPP_
#define OS_RAWTIME_HPP_

#include <FpConfig.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Fw/Time/TimeInterval.hpp>
#include <Os/Os.hpp>


namespace Os {

struct RawTimeHandle {};

class RawTime; // Forward declaration

class RawTimeInterface : public Fw::Serializable {
  
  public:

    // Serialization size for RawTime objects, configured in config/FpConfig.h
    static const FwSizeType SERIALIZED_SIZE = FW_RAW_TIME_SERIALIZATION_MAX_SIZE;

    enum Status {
      OP_OK, //!<  Operation was successful
      OP_OVERFLOW, //!< Operation result caused an overflow
      INVALID_PARAMS, //!< Parameters invalid for current platform
      OTHER_ERROR //!< All other errors
    };

    //! \brief default constructor
    RawTimeInterface() = default;

    //! \brief default virtual destructor
    virtual ~RawTimeInterface() = default;

    //! \brief return the underlying RawTime handle (implementation specific)
    //! \return internal RawTime handle representation
    virtual RawTimeHandle* getHandle() = 0;

    //! \brief provide a pointer to a RawTime delegate object
    static RawTimeInterface* getDelegate(RawTimeHandleStorage& aligned_new_memory, const RawTimeInterface* to_copy=nullptr);

    // ------------------------------------------------------------------
    // RawTime operations to be implemented by an OSAL implementation
    // ------------------------------------------------------------------

    //! \brief Get the current time.
    //!
    //! This function retrieves the current time and stores it in the RawTime object.
    //! Each implementation should define its RawTimeHandle type for storing the time.
    //!
    //! \return Status indicating the result of the operation.
    virtual Status now() = 0;

    //! \brief Calculate the time interval between this and another raw time.
    //!
    //! This function calculates the time interval between the current raw time and another
    //! specified raw time. The result is stored in the provided (output) interval object.
    //!
    //! \param other The other RawTimeHandle to compare against.
    //! \param interval Output parameter to store the calculated time interval.
    //! \return Status indicating the result of the operation.
    virtual Status getTimeInterval(const Os::RawTime& other, Fw::TimeInterval& interval) const = 0;

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
    virtual Fw::SerializeStatus serialize(Fw::SerializeBufferBase& buffer) const = 0;

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
    virtual Fw::SerializeStatus deserialize(Fw::SerializeBufferBase& buffer) = 0;

};

class RawTime final : public RawTimeInterface {
  public:
    RawTime();         //!<  Constructor
    ~RawTime() final;  //!<  Destructor
    
    //! \brief copy constructor that copies the internal representation
    RawTime(const RawTime& other);

    //! \brief assignment operator that copies the internal representation
    RawTime& operator=(const RawTime& other);

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

    // ------------------------------------------------------------
    // Common functions built on top of OS-specific functions
    // ------------------------------------------------------------

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
    Status getDiffUsec(const RawTime& other, U32& result) const;


  private:
    // This section is used to store the implementation-defined RawTime handle. To Os::RawTime and fprime, this type is
    // opaque and thus normal allocation cannot be done. Instead, we allow the implementor to store the handle in
    // the byte-array here and set `m_handle_storage` to that address for storage.
    //
    alignas(FW_HANDLE_ALIGNMENT) RawTimeHandleStorage m_handle_storage;  //!< RawTime handle storage
    RawTimeInterface& m_delegate;                                   //!< Delegate for the real implementation
};
}  // namespace Os

#endif
