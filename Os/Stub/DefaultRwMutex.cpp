// ======================================================================
// \title Os/Stub/DefaultRwMutex.cpp
// \brief sets default Os::RwMutex to no-op stub implementation via linker
// ======================================================================
#include "Os/Delegate.hpp"
#include "Os/Stub/RwMutex.hpp"
namespace Os {

//! \brief get a delegate for RwMutexInterface that intercepts calls for stub file usage
//! \param aligned_new_memory: aligned memory to fill
//! \param to_copy: pointer to copy-constructor input
//! \return: pointer to delegate
RwMutexInterface* RwMutexInterface::getDelegate(RwMutexHandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<RwMutexInterface, Os::Stub::Mutex::StubRwMutex>(aligned_new_memory);
}

}  // namespace Os
