// ======================================================================
// \title Os/Cpu.hpp
// \brief common function definitions for Os::Cpu
// ======================================================================
#include "Os/Os.hpp"

#ifndef OS_CPU_HPP_
#define OS_CPU_HPP_

namespace Os {

//! \brief Cpu variable handle parent
class CpuHandle {};

//! \brief interface for cpu implementation
class CpuInterface {
  public:
    using Status = Os::Generic::Status;
    using Ticks = Os::Generic::UsedTotal;

    //! Default constructor
    CpuInterface() = default;
    //! Default destructor
    virtual ~CpuInterface() = default;

    //! \brief copy constructor is forbidden
    CpuInterface(const CpuInterface& other) = delete;

    //! \brief assignment operator is forbidden
    virtual CpuInterface& operator=(const CpuInterface& other) = delete;

    //! \brief Request the count of the CPUs detected by the system
    //!
    //! \param cpu_count: (output) filled with CPU count on system
    //! \return: OP_OK with valid CPU count, ERROR when error occurs
    //!
    virtual Status _getCount(FwSizeType & cpu_count) = 0;

    //! \brief Get the CPU tick information for a given CPU
    //!
    //! CPU ticks represent a small time slice of processor time. This will retrieve the used CPU ticks and total
    //! ticks for a given CPU. This information in a running accumulation and thus a sample-to-sample
    //! differencing is needed to see the 'realtime' changing load. This shall be done by the caller.
    //!
    //! \param ticks: (output) filled with the tick information for the given CPU
    //! \param cpu_index: index for CPU to read. Default: 0
    //! \return:  ERROR when error occurs, OK otherwise.
    //!
    virtual Status _getTicks(Ticks& ticks, FwSizeType cpu_index) = 0;

    //! \brief return the underlying cpu handle (implementation specific).
    //! \return internal task handle representation
    virtual CpuHandle* getHandle() = 0;

    //! \brief provide a pointer to a Mutex delegate object
    static CpuInterface* getDelegate(CpuHandleStorage& aligned_new_memory);
};

//! \brief cpu implementation
class Cpu final : public CpuInterface {
  public:
    //! \brief default constructor
    Cpu();

    //! \brief default virtual destructor
    ~Cpu() final;

    //! \brief copy constructor is forbidden
    Cpu(const CpuInterface& other) = delete;

    //! \brief copy constructor is forbidden
    Cpu(const CpuInterface* other) = delete;

    //! \brief assignment operator is forbidden
    CpuInterface& operator=(const CpuInterface& other) override = delete;

    //-----------------------------------------------------------------------------
    // Interface methods
    //-----------------------------------------------------------------------------

    //! \brief initialize the singleton
    void init();

    //! \brief return singleton
    static Cpu& getSingleton();

    //-----------------------------------------------------------------------------
    // Delegating methods
    //-----------------------------------------------------------------------------

    //! \brief Request the count of the CPUs detected by the system
    //!
    //! This method wraps delegates to the underlying implementation.
    //!
    //! \param cpu_count: (output) filled with CPU count on system
    //! \return: OP_OK with valid CPU count, ERROR when error occurs
    //!
    Status _getCount(FwSizeType& cpu_count) override;

    //! \brief Get the CPU tick information for a given CPU
    //!
    //! CPU ticks represent a small time slice of processor time. This will retrieve the used CPU ticks and total
    //! ticks for a given CPU. This information in a running accumulation and thus a sample-to-sample
    //! differencing is needed to see the 'realtime' changing load. This shall be done by the caller. This method wraps
    //! delegates to the underlying implementation.
    //!
    //! \param ticks: (output) filled with the tick information for the given CPU
    //! \param cpu_index: index for CPU to read. Default: 0
    //! \return:  ERROR when error occurs, OK otherwise.
    //!
    Status _getTicks(Ticks& ticks, FwSizeType cpu_index) override;

    //! \brief return the underlying cpu handle (implementation specific).
    //! \return internal task handle representation
    CpuHandle* getHandle() override;

    //-----------------------------------------------------------------------------
    // Static interface (singleton) methods
    //-----------------------------------------------------------------------------

    //! \brief Request the count of the CPUs detected by the system
    //!
    //! This method wraps a singleton implementation.
    //!
    //! \param cpu_count: (output) filled with CPU count on system
    //! \return: OP_OK with valid CPU count, ERROR when error occurs
    //!
    static  Status getCount(FwSizeType& cpu_count);

    //! \brief Get the CPU tick information for a given CPU
    //!
    //! CPU ticks represent a small time slice of processor time. This will retrieve the used CPU ticks and total
    //! ticks for a given CPU. This information in a running accumulation and thus a sample-to-sample
    //! differencing is needed to see the 'realtime' changing load. This shall be done by the caller. This method wraps
    //! a singleton implementation.
    //!
    //! \param ticks: (output) filled with the tick information for the given CPU
    //! \param cpu_index: index for CPU to read. Default: 0
    //! \return:  ERROR when error occurs, OK otherwise.
    //!
    static Status getTicks(Ticks& ticks, FwSizeType cpu_index);

  private:

    // This section is used to store the implementation-defined file handle. To Os::File and fprime, this type is
    // opaque and thus normal allocation cannot be done. Instead, we allow the implementor to store then handle in
    // the byte-array here and set `handle` to that address for storage.
    alignas(FW_HANDLE_ALIGNMENT) CpuHandleStorage m_handle_storage; //!< Storage for aligned data
    CpuInterface& m_delegate;                                       //!< Delegate for the real implementation
};
}
#endif //OS_CONDITION_HPP_
