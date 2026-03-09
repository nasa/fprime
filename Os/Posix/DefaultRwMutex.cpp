// ======================================================================
// \title Os/Posix/DefaultRwMutex.cpp
// \brief sets default Os::RwMutex Posix implementation via linker
// ======================================================================
#include "Os/Delegate.hpp"
#include "Os/Posix/RwMutex.hpp"
namespace Os {

//! \brief get a delegate for RwMutexInterface that intercepts calls for Posix
//! \param aligned_new_memory: aligned memory to fill
//! \return: pointer to delegate
RwMutexInterface* RwMutexInterface::getDelegate(RwMutexHandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<RwMutexInterface, Os::Posix::Mutex::PosixRwMutex>(aligned_new_memory);
}

}  // namespace Os
