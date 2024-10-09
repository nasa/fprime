// ======================================================================
// \title Os/Darwin/Cpu.cpp
// \brief Darwin implementation for Os::Cpu
// ======================================================================
#include <Os/Darwin/Cpu.hpp>
#include <Fw/Types/Assert.hpp>
#include <mach/mach_error.h>
#include <mach/mach_host.h>
#include <mach/mach_init.h>
#include <mach/mach_types.h>
#include <mach/message.h>

namespace Os {
namespace Darwin {
namespace Cpu {

//! \brief helper around raw CPU capture API
//!
//! Calls for the CPU information from the machine, improving readability in cpu_by_index
//!
//! \param cpu_load_info: filled with CPU data
//! \param cpu_count: filled with CPU count
//!
//! \return success/failure using kern_return_t
kern_return_t cpu_data_helper(processor_cpu_load_info_t& cpu_load_info, FwSizeType& cpu_count) {
    static_assert(std::numeric_limits<FwSizeType>::max() >= std::numeric_limits<natural_t>::max(),
                  "FwSizeType cannot hold natural_t values");
    natural_t cpu_count_natural;
    mach_msg_type_number_t processor_msg_count;
    kern_return_t stat = host_processor_info(mach_host_self(), PROCESSOR_CPU_LOAD_INFO, &cpu_count_natural,
                                             reinterpret_cast<processor_info_array_t*>(&cpu_load_info), &processor_msg_count);
    cpu_count = cpu_count_natural;
    return stat;
}

//! \brief Query for a single CPU's ticks information
//!
//! Queries all CPU information but only deals with a single CPU's output. This is done because the load average is
//! tracked sample to sample and the call pattern is cpu0, cpu1, ..., cpu last, wait for sample window, cpu0, ... and
//! thus each call should update one CPU's sample or only the last cpu will have the benefit of the sampling window.
//!
//! \param cpu_index: index of current CPU being queried
//! \param used: filled with CPU's used ticks count
//! \param total: filled with CPU's total ticks
//!
//! \return success/failure using kern_return_t
kern_return_t cpu_by_index(FwSizeType cpu_index, FwSizeType& used, FwSizeType& total) {
    processor_cpu_load_info_t cpu_load_info;
    FwSizeType cpu_count = 0;
    kern_return_t status = cpu_data_helper(cpu_load_info, cpu_count);

    // Failure for CPU index
    if (cpu_count <= cpu_index) {
        status = KERN_FAILURE;
    } else if (KERN_SUCCESS == status) {
        processor_cpu_load_info per_cpu_info = cpu_load_info[cpu_index];

        // Total the ticks across the different states: idle, system, user, etc...
        total = 0;
        for (FwSizeType i = 0; i < CPU_STATE_MAX; i++) {
            total += per_cpu_info.cpu_ticks[i];
        }
        used = total - per_cpu_info.cpu_ticks[CPU_STATE_IDLE];
    }
    return status;
}

CpuInterface::Status DarwinCpu::_getCount(FwSizeType& cpu_count) {
    processor_cpu_load_info_t cpu_load_info;
    if (KERN_SUCCESS == cpu_data_helper(cpu_load_info, cpu_count)) {
        return Status::OP_OK;
    }
    cpu_count = 0;
    return Status::ERROR;
}

CpuInterface::Status DarwinCpu::_getTicks(Os::Cpu::Ticks& ticks, FwSizeType cpu_index) {
    kern_return_t status = cpu_by_index(cpu_index, ticks.used, ticks.total);
    if (KERN_SUCCESS == status) {
        return Status::OP_OK;
    }
    ticks.total = 1;
    ticks.used = 1;
    return Status::ERROR;
}

CpuHandle* DarwinCpu::getHandle() {
    return &this->m_handle;
}

} // namespace Cpu
} // namespace Darwin
} // namespace Os
