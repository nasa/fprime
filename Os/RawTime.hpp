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

class RawTimeInterface : public Fw::Serializable {
  
  public:

    // Serialization size for RawTime objects
    static const FwSizeType SERIALIZED_SIZE = 2 * sizeof(U32); // TODO: document why + this is a limitation

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
    static RawTimeInterface* getDelegate(HandleStorage& aligned_new_memory, const RawTimeInterface* to_copy=nullptr);

    // ------------------------------------------------------------------
    // RawTime operations to be implemented by an OSAL implementation
    // ------------------------------------------------------------------
    //! \brief Get current time and store it in the object
    //!
    //! Store the current time in implementation-specific RawTimeHandle
    //!
    //! \return `Status` status of the operation.
    virtual Status getRawTime() = 0;

    //! \brief Calculate the time interval between the current raw time and another raw time.
    //! \param other The other RawTimeHandle to compare against.
    //! \param interval Output: the calculated time interval.
    //! \return Status of the operation.
    virtual Status getTimeInterval(const RawTimeHandle& other, Fw::TimeInterval& interval) const = 0;

    //! \brief Serialize the contents of the RawTimeInterface object.
    //!
    //! NOTE: Serialization size is limited to 2 * sizeof(U32).
    //!
    //! \param buffer The buffer to serialize the contents into.
    //! \return `Fw::SerializeStatus` status of the serialization.
    virtual Fw::SerializeStatus serialize(Fw::SerializeBufferBase& buffer) const = 0;

    //! \brief Deserialize the contents of the RawTimeInterface object.
    //!
    //! NOTE: Serialization size is limited to 2 * sizeof(U32).
    //!
    //! \param buffer The buffer to deserialize the contents from.
    //! \return `Fw::SerializeStatus` status of the deserialization.
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

    Status getRawTime() override;
    Status getTimeInterval(const RawTimeHandle& other, Fw::TimeInterval& interval) const override;  //!<  docs

    Fw::SerializeStatus serialize(Fw::SerializeBufferBase& buffer) const override;  //!< serialize contents
    Fw::SerializeStatus deserialize(Fw::SerializeBufferBase& buffer) override;      //!< deserialize to contents

    // TODO: add toFwTime() ?? and fromFwTime() ??


    //------------ Common Functions ------------
    Status getDiffUsec(const RawTime& other, U32& result) const;
    Status getTimeInterval(const RawTime& other, Fw::TimeInterval& result_interval) const;  //!<  docs


  private:
    // This section is used to store the implementation-defined RawTime handle. To Os::RawTime and fprime, this type is
    // opaque and thus normal allocation cannot be done. Instead, we allow the implementor to store the handle in
    // the byte-array here and set `m_handle_storage` to that address for storage.
    //
    alignas(FW_HANDLE_ALIGNMENT) HandleStorage m_handle_storage;  //!< RawTime handle storage
    RawTimeInterface& m_delegate;                                   //!< Delegate for the real implementation
};
}  // namespace Os

#endif
