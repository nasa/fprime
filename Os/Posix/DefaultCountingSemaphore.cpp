// ======================================================================
// \title Os/Posix/DefaultCountingSemaphore.cpp
// \brief sets default Os::CountingSemaphore Posix implementation via linker
// ======================================================================
#include "Os/Delegate.hpp"
#include "Os/Posix/CountingSemaphore.hpp"

namespace Os {

CountingSemaphoreInterface* CountingSemaphoreInterface::getDelegate(CountingSemaphoreHandleStorage& aligned_new_memory,
                                                                    U32 initial_count) {
    return Os::Delegate::makeDelegate<CountingSemaphoreInterface, Os::Posix::Semaphore::PosixCountingSemaphore>(
        aligned_new_memory, initial_count);
}
}  // namespace Os
