// ======================================================================
// \title Os/Stub/DefaultCountingSemaphore.cpp
// \brief sets default Os::CountingSemaphore to stub implementation via linker
// ======================================================================
#include "Os/Delegate.hpp"
#include "Os/Stub/CountingSemaphore.hpp"

namespace Os {

CountingSemaphoreInterface* CountingSemaphoreInterface::getDelegate(CountingSemaphoreHandleStorage& aligned_new_memory,
                                                                    U32 initial_count) {
    return Os::Delegate::makeDelegate<CountingSemaphoreInterface, Os::Stub::Semaphore::StubCountingSemaphore>(
        aligned_new_memory, initial_count);
}
}  // namespace Os
