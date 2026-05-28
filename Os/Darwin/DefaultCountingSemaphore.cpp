// ======================================================================
// \title Os/Darwin/DefaultCountingSemaphore.cpp
// \brief sets default Os::CountingSemaphore Darwin implementation via linker
// ======================================================================
#include "Os/Darwin/CountingSemaphore.hpp"
#include "Os/Delegate.hpp"

namespace Os {

CountingSemaphoreInterface* CountingSemaphoreInterface::getDelegate(CountingSemaphoreHandleStorage& aligned_new_memory,
                                                                    U32 initial_count) {
    return new (aligned_new_memory) Os::Darwin::Semaphore::DarwinCountingSemaphore(initial_count);
}
}  // namespace Os
