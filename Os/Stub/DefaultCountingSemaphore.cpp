// ======================================================================
// \title Os/Stub/DefaultCountingSemaphore.cpp
// \brief sets default Os::CountingSemaphore to stub implementation via linker
// ======================================================================
#include "Os/Delegate.hpp"
#include "Os/Stub/CountingSemaphore.hpp"

namespace Os {

CountingSemaphoreInterface* CountingSemaphoreInterface::getDelegate(CountingSemaphoreHandleStorage& aligned_new_memory,
                                                                    U32 initial_count) {
    static_assert(std::is_base_of<CountingSemaphoreInterface, Os::Stub::Semaphore::StubCountingSemaphore>::value,
                  "Implementation must derive from CountingSemaphoreInterface");
    static_assert(sizeof(Os::Stub::Semaphore::StubCountingSemaphore) <= sizeof(CountingSemaphoreHandleStorage),
                  "Handle size not large enough");
    static_assert((FW_HANDLE_ALIGNMENT % alignof(Os::Stub::Semaphore::StubCountingSemaphore)) == 0,
                  "Handle alignment invalid");
    auto* ptr = new (aligned_new_memory) Os::Stub::Semaphore::StubCountingSemaphore(initial_count);
    FW_ASSERT(ptr != nullptr);
    return ptr;
}
}  // namespace Os
