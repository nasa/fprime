// ======================================================================
// \title Os/Posix/DefaultCountingSemaphore.cpp
// \brief sets default Os::CountingSemaphore Posix implementation via linker
// ======================================================================
#include "Os/Delegate.hpp"
#include "Os/Posix/CountingSemaphore.hpp"

namespace Os {

CountingSemaphoreInterface* CountingSemaphoreInterface::getDelegate(CountingSemaphoreHandleStorage& aligned_new_memory,
                                                                    U32 initial_count) {
    return new (aligned_new_memory) Os::Posix::Semaphore::PosixCountingSemaphore(initial_count);
}
}  // namespace Os
