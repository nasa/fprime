// ======================================================================
// \title Os/Stub/Cpu.hpp
// \brief stub implementation for Os::Cpu, header and test definitions
// ======================================================================
#include <cstdio>
#include <Os/Cpu.hpp>
#ifndef OS_Stub_Cpu_HPP
#define OS_Stub_Cpu_HPP

namespace Os {
namespace Stub {
namespace Cpu {

//! CpuHandle class definition for stub implementations.
//!
struct StubCpuHandle : public CpuHandle {
};

//! \brief stub implementation of Os::CpuInterface
//!
//! Stub implementation of `CpuInterface` for use as a delegate class handling stub console operations.
//!
class StubCpu : public CpuInterface {
  public:
    //! \brief constructor
    //!
    StubCpu() = default;

    //! \brief copy constructor
    StubCpu(const StubCpu& other) = delete;

    //! \brief default copy assignment
    CpuInterface& operator=(const CpuInterface& other) override = delete;

    //! \brief destructor
    //!
    ~StubCpu() override = default;

    // ------------------------------------
    // Functions overrides
    // ------------------------------------
  public:

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

    //! \brief returns the raw console handle
    //!
    //! Gets the raw console handle from the implementation. Note: users must include the implementation specific
    //! header to make any real use of this handle. Otherwise it will be as an opaque type.
    //!
    //! \return raw console handle
    //!
    CpuHandle *getHandle() override;
  private:
    //! File handle for PosixFile
    StubCpuHandle m_handle;
};
} // namespace Cpu
} // namespace Stub
} // namespace Os

#endif // OS_Stub_Cpu_HPP
