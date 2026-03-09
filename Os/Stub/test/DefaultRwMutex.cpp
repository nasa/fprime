// ======================================================================
// \title Os/Stub/test/DefaultRwMutex.cpp
// \brief sets default Os::RwMutex to test stub implementation via linker
// ======================================================================
#include "Os/Delegate.hpp"
#include "Os/Stub/test/RwMutex.hpp"

namespace Os {

//! \brief get a delegate for RwMutexInterface that intercepts calls for stub file usage
//! \param aligned_new_memory: aligned memory to fill
//! \param to_copy: pointer to copy-constructor input
//! \return: pointer to delegate
RwMutexInterface* RwMutexInterface::getDelegate(RwMutexHandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<RwMutexInterface, Os::Stub::RwMutex::Test::TestRwMutex>(aligned_new_memory);
}

}  // namespace Os
