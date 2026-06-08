// ======================================================================
// \title Os/Stub/test/DefaultCountingSemaphore.cpp
// \brief sets default Os::CountingSemaphore to test stub implementation via linker
// ======================================================================
#include "Os/Delegate.hpp"
#include "Os/Stub/test/CountingSemaphore.hpp"

namespace Os {

//! \brief get a delegate for CountingSemaphore that intercepts calls for stub test CountingSemaphore usage
//! \param aligned_new_memory: aligned memory to fill
//! \return: pointer to delegate
CountingSemaphoreInterface* CountingSemaphoreInterface::getDelegate(
    CountingSemaphoreHandleStorage& aligned_placement_new_memory,
    U32 initial_count) {
    static_assert(
        std::is_base_of<CountingSemaphoreInterface, Os::Stub::CountingSemaphore::Test::TestCountingSemaphore>::value,
        "Implementation must derive from CountingSemaphoreInterface");
    static_assert(
        sizeof(Os::Stub::CountingSemaphore::Test::TestCountingSemaphore) <= sizeof(CountingSemaphoreHandleStorage),
        "Handle size not large enough");
    static_assert((FW_HANDLE_ALIGNMENT % alignof(Os::Stub::CountingSemaphore::Test::TestCountingSemaphore)) == 0,
                  "Handle alignment invalid");
    auto* ptr =
        new (aligned_placement_new_memory) Os::Stub::CountingSemaphore::Test::TestCountingSemaphore(initial_count);
    FW_ASSERT(ptr != nullptr);
    return ptr;
}
}  // namespace Os
