// ======================================================================
// \title Os/Darwin/DefaultCountingSemaphore.cpp
// \brief sets default Os::CountingSemaphore Darwin implementation via linker
// ======================================================================
#include "Os/Darwin/CountingSemaphore.hpp"
#include "Os/Delegate.hpp"

namespace Os {

CountingSemaphoreInterface* CountingSemaphoreInterface::getDelegate(CountingSemaphoreHandleStorage& aligned_new_memory,
                                                                    U32 initial_count) {
    static_assert(std::is_base_of<CountingSemaphoreInterface, Os::Darwin::Semaphore::DarwinCountingSemaphore>::value,
                  "Implementation must derive from CountingSemaphoreInterface");
    static_assert(sizeof(Os::Darwin::Semaphore::DarwinCountingSemaphore) <= sizeof(CountingSemaphoreHandleStorage),
                  "Handle size not large enough");
    static_assert((FW_HANDLE_ALIGNMENT % alignof(Os::Darwin::Semaphore::DarwinCountingSemaphore)) == 0,
                  "Handle alignment invalid");
    auto* ptr = new (aligned_new_memory) Os::Darwin::Semaphore::DarwinCountingSemaphore(initial_count);
    FW_ASSERT(ptr != nullptr);
    return ptr;
}
}  // namespace Os
