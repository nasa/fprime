// ======================================================================
// \title Os/Darwin/error.cpp
// \brief Darwin dispatch semaphore return status conversion implementation
// ======================================================================
#include "Os/Darwin/error.hpp"

namespace Os {
namespace Darwin {

Os::CountingSemaphore::Status dispatch_result_to_semaphore_status(long dispatch_result) {
    // dispatch_semaphore_wait returns 0 on success, non-zero on timeout
    if (dispatch_result == 0) {
        return Os::CountingSemaphore::Status::OP_OK;
    }
    return Os::CountingSemaphore::Status::ERROR_TIMEOUT;
}

}  // namespace Darwin
}  // namespace Os
