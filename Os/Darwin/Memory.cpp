// ======================================================================
// \title Os/Darwin/Memory.cpp
// \brief Darwin implementation for Os::Memory
// ======================================================================
#include <Os/Darwin/Memory.hpp>
#include <mach/mach_error.h>
#include <mach/mach_host.h>
#include <mach/mach_init.h>
#include <mach/mach_types.h>
#include <mach/message.h>

namespace Os {
namespace Darwin {
namespace Memory {

/**
 * \brief reads macOS virtual memory statistics for memory calculation
 *
 * Queries the macOS kernel for virtual memory information. These items are returned in units of page-size and are
 * then converted back into bytes.
 *
 * Thanks to: https://stackoverflow.com/questions/8782228/retrieve-ram-info-on-a-mac
 *
 * \param used: used memory in bytes
 * \param total: total memory in bytes
 * \return: kern_return_t with success/failure straight from the kernel
 */
kern_return_t vm_stat_helper(FwSizeType& used, FwSizeType& total) {
    mach_msg_type_number_t count = HOST_VM_INFO_COUNT;
    vm_statistics_data_t vmstat;
    vm_size_t vmsize;

    kern_return_t status1 = host_statistics(mach_host_self(), HOST_VM_INFO, reinterpret_cast<host_info_t>(&vmstat), &count);
    kern_return_t status2 = host_page_size(mach_host_self(), &vmsize);

    if (KERN_SUCCESS == status1 and KERN_SUCCESS == status2) {
        // Wired (permanently in RAM), active (recently used), and inactive (not recently used) pages
        used = vmstat.wire_count + vmstat.active_count + vmstat.inactive_count;
        total = used + vmstat.free_count;

        // Pages to totals
        used *= vmsize;
        total *= vmsize;
    }
    return (status1 == KERN_SUCCESS) ? status2 : status1;
}


MemoryInterface::Status DarwinMemory::_getUsage(Os::Memory::Usage& memory_usage) {
    // Call out VM helper
    if (KERN_SUCCESS == vm_stat_helper(memory_usage.used, memory_usage.total)) {
        return Status::OP_OK;
    }
    // Force something sensible, while preventing divide by zero
    memory_usage.total = 1;
    memory_usage.used = 1;
    return Status::ERROR;
}

MemoryHandle* DarwinMemory::getHandle() {
    return &this->m_handle;
}

} // namespace Memory
} // namespace Darwin
} // namespace Os
