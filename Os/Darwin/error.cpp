// ======================================================================
// \title Os/Darwin/error.cpp
// \brief Darwin/Mach kernel return status conversion implementation
// ======================================================================
#include "Os/Darwin/error.hpp"
#include "Fw/Types/Assert.hpp"

#include <mach/kern_return.h>

namespace Os {
namespace Darwin {

Os::CountingSemaphore::Status kern_return_to_semaphore_status(long kern_status) {
    Os::CountingSemaphore::Status status = Os::CountingSemaphore::Status::ERROR_OTHER;
    
    switch (kern_status) {
        case KERN_SUCCESS:
            status = Os::CountingSemaphore::Status::OP_OK;
            break;
        case KERN_OPERATION_TIMED_OUT:
            status = Os::CountingSemaphore::Status::ERROR_TIMEOUT;
            break;
        case KERN_INVALID_ARGUMENT:
        case KERN_SEMAPHORE_DESTROYED:
            status = Os::CountingSemaphore::Status::ERROR_INVALID;
            break;
        case KERN_ABORTED:
        case KERN_TERMINATED:
        default:
            status = Os::CountingSemaphore::Status::ERROR_OTHER;
            break;
    }
    
    FW_ASSERT(kern_status >= 0);
    return status;
}

}  // namespace Darwin
}  // namespace Os
