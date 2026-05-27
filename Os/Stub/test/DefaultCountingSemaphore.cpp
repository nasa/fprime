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
    U32 initial_count,
    int pshared) {
    return new (aligned_placement_new_memory)
        Os::Stub::CountingSemaphore::Test::TestCountingSemaphore(initial_count, pshared);
}
}  // namespace Os
