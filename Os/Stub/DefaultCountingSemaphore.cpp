// ======================================================================
// \title Os/Stub/DefaultCountingSemaphore.cpp
// \brief sets default Os::CountingSemaphore to stub implementation via linker
// ======================================================================
#include "Os/Delegate.hpp"
#include "Os/Stub/CountingSemaphore.hpp"

namespace Os {

CountingSemaphoreInterface* CountingSemaphoreInterface::getDelegate(CountingSemaphoreHandleStorage& aligned_new_memory,
                                                                    U32 initial_count,
                                                                    int pshared) {
    return new (aligned_new_memory) Os::Stub::Semaphore::StubCountingSemaphore(initial_count, pshared);
}
}  // namespace Os
