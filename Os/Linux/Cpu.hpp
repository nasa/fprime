// ======================================================================
// \title Os/Linux/Cpu.hpp
// \brief Linux implementation for Os::Cpu, header and test definitions
// ======================================================================
#include <Os/Cpu.hpp>
#include <Os/File.hpp>
#ifndef OS_Linux_Cpu_HPP
#define OS_Linux_Cpu_HPP

namespace Os {
namespace Linux {
namespace Cpu {

//! CpuHandle class definition for stub implementations.
//!
struct LinuxCpuHandle : public CpuHandle {};

//! \brief stub implementation of Os::CpuInterface
//!
//! Linux implementation of `CpuInterface` for use as a delegate class handling stub console operations.
//!
class LinuxCpu : public CpuInterface {
  public:
    //! \brief constructor
    //!
    LinuxCpu() = default;

    //! \brief copy constructor
    LinuxCpu(const LinuxCpu& other) = delete;

    //! \brief default copy assignment
    CpuInterface& operator=(const CpuInterface& other) override = delete;

    //! \brief destructor
    //!
    ~LinuxCpu() override = default;

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
    Status _getTicks(Os::Cpu::Ticks& ticks, FwSizeType cpu_index) override;

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
    LinuxCpuHandle m_handle;
};
} // namespace Cpu
} // namespace Linux
} // namespace Os

#endif // OS_Linux_Cpu_HPP
