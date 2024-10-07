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

    static const FwSizeType SERIALIZED_SIZE;

    enum Status { 
      OP_OK, //!<  Operation was successful
      OP_OVERFLOW, //!< Operation result caused an overflow
      ERROR_OTHER //!< All other errors
    };

    //! \brief default constructor
    RawTimeInterface() = default;

    //! \brief default virtual destructor
    virtual ~RawTimeInterface() = default;

    //! \brief return the underlying RawTime handle (implementation specific)
    //! \return internal RawTime handle representation
    virtual RawTimeHandle* getHandle() = 0;

    //! \brief provide a pointer to a RawTime delegate object
    // TODO: docs
    static RawTimeInterface* getDelegate(HandleStorage& aligned_new_memory, const RawTimeInterface* to_copy=nullptr);  // TODO

    // ------------------------------------------------------------------
    // RawTime operations to be implemented by an OSAL implementation
    // ------------------------------------------------------------------
    virtual Status getRawTime() = 0;                 //!<  docs
    virtual Status getDiffUsec(const RawTimeHandle& other, U32& result) const = 0;  //!<  docs
    virtual Status getTimeInterval(const RawTimeHandle& other, Fw::TimeInterval& interval) const = 0;  //!<  docs

    virtual Fw::SerializeStatus serialize(Fw::SerializeBufferBase& buffer) const = 0;  //!< serialize contents
    virtual Fw::SerializeStatus deserialize(Fw::SerializeBufferBase& buffer) = 0;      //!< deserialize to contents

};

class RawTime final : public RawTimeInterface {
  public:
    RawTime();         //!<  Constructor. TODO: RawTime initalization??
    ~RawTime() final;  //!<  Destructor
    
    //! \brief copy constructor that copies the internal representation
    RawTime(const RawTime& other);

    //! \brief assignment operator that copies the internal representation
    RawTime& operator=(const RawTime& other);

    //! \brief return the underlying RawTime handle (implementation specific)
    //! \return internal RawTime handle representation
    RawTimeHandle* getHandle() override;

    Status getRawTime() override;
    Status getDiffUsec(const RawTimeHandle& other, U32& result) const override;
    Status getTimeInterval(const RawTimeHandle& other, Fw::TimeInterval& interval) const override;  //!<  docs

    Fw::SerializeStatus serialize(Fw::SerializeBufferBase& buffer) const override;  //!< serialize contents
    Fw::SerializeStatus deserialize(Fw::SerializeBufferBase& buffer) override;      //!< deserialize to contents

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
