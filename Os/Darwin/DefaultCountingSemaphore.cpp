// ======================================================================
// \title Os/Darwin/DefaultCountingSemaphore.cpp
// \brief sets default Os::CountingSemaphore Darwin implementation via linker
// ======================================================================
#include "Os/Delegate.hpp"
#include "Os/Darwin/CountingSemaphore.hpp"

namespace Os {

CountingSemaphoreInterface* CountingSemaphoreInterface::getDelegate(CountingSemaphoreHandleStorage& aligned_new_memory,
                                                                    U32 initial_count,
                                                                    int pshared) {
    return new (aligned_new_memory) Os::Darwin::Semaphore::DarwinCountingSemaphore(initial_count, pshared);
}
}  // namespace Os
