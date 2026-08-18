// ======================================================================
// \title Os/Posix/DefaultCountingSemaphore.cpp
// \brief sets default Os::CountingSemaphore Posix implementation via linker
// ======================================================================
#include "Os/Delegate.hpp"
#include "Os/Posix/CountingSemaphore.hpp"

namespace Os {

CountingSemaphoreInterface* CountingSemaphoreInterface::getDelegate(CountingSemaphoreHandleStorage& aligned_new_memory,
                                                                    U32 initial_count) {
    static_assert(std::is_base_of<CountingSemaphoreInterface, Os::Posix::Semaphore::PosixCountingSemaphore>::value,
                  "Implementation must derive from CountingSemaphoreInterface");
    static_assert(sizeof(Os::Posix::Semaphore::PosixCountingSemaphore) <= sizeof(CountingSemaphoreHandleStorage),
                  "Handle size not large enough");
    static_assert((FW_HANDLE_ALIGNMENT % alignof(Os::Posix::Semaphore::PosixCountingSemaphore)) == 0,
                  "Handle alignment invalid");
    auto* ptr = new (aligned_new_memory) Os::Posix::Semaphore::PosixCountingSemaphore(initial_count);
    FW_ASSERT(ptr != nullptr);
    return ptr;
}
}  // namespace Os
