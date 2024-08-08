// ======================================================================
// \title Os/Stub/DefaultMutex.cpp
// \brief sets default Os::Mutex to no-op stub implementation via linker
// ======================================================================
#include "Os/Stub/Mutex.hpp"
#include "Os/Delegate.hpp"
namespace Os {

//! \brief get a delegate for MutexInterface that intercepts calls for stub file usage
//! \param aligned_new_memory: aligned memory to fill
//! \param to_copy: pointer to copy-constructor input
//! \return: pointer to delegate
MutexInterface *MutexInterface::getDelegate(HandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<MutexInterface, Os::Stub::Mutex::StubMutex>(
            aligned_new_memory
    );
}
}
