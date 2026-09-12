// ======================================================================
// \title Os/CpuInterface.hpp
// \brief Os::CpuHandle and Os::CpuInterface definitions
// ======================================================================
#ifndef OS_CPUINTERFACE_HPP_
#define OS_CPUINTERFACE_HPP_

#include "Os/Os.hpp"
#include "config/OsDelegateCpu.hpp"

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
    virtual CpuInterface& operator=(const CpuInterface& other) = delete;  //  NO_CODESONAR (cpp:S3657)

    //! \brief Request the count of the CPUs detected by the system
    //!
    //! \param cpu_count: (output) filled with CPU count on system
    //! \return: OP_OK with valid CPU count, ERROR when error occurs
    //!
    virtual Status _getCount(FwSizeType& cpu_count) = 0;

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

    //! \brief provide a pointer to a Cpu delegate object
    static CpuInterface* getDelegate(CpuHandleStorage& aligned_new_memory);

    // ------------------------------------------------------------------
    // Convenience static wrappers
    // ------------------------------------------------------------------
    // These are implemented in terms of the virtual instance functions above,
    // and are inherited (via ordinary class-scope lookup) by every concrete
    // Os::Cpu backend, regardless of whether that backend is a link-time
    // delegate or a direct compile-time alias.

    //! \brief Request the count of the CPUs detected by the system
    //!
    //! \param cpu_count: (output) filled with CPU count on system
    //! \return: OP_OK with valid CPU count, ERROR when error occurs
    //!
    static Status getCount(FwSizeType& cpu_count);

    //! \brief Get the CPU tick information for a given CPU
    //!
    //! \param ticks: (output) filled with the tick information for the given CPU
    //! \param cpu_index: index for CPU to read. Default: 0
    //! \return:  ERROR when error occurs, OK otherwise.
    //!
    static Status getTicks(Ticks& ticks, FwSizeType cpu_index);
};
}  // namespace Os
#endif  // OS_CPUINTERFACE_HPP_
