// ======================================================================
// \title Os/Posix/DefaultMutex.cpp
// \brief sets default Os::Mutex Posix implementation via linker
// ======================================================================
#include "Os/Posix/Mutex.hpp"
#include "Os/Delegate.hpp"
namespace Os {

//! \brief get a delegate for MutexInterface that intercepts calls for Posix
//! \param aligned_new_memory: aligned memory to fill
//! \return: pointer to delegate
MutexInterface *MutexInterface::getDelegate(HandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<MutexInterface, Os::Posix::Mutex::PosixMutex>(
            aligned_new_memory
    );
}
}
