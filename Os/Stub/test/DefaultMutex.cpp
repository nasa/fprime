// ======================================================================
// \title Os/Stub/test/DefaultMutex.cpp
// \brief sets default Os::Mutex to test stub implementation via linker
// ======================================================================
#include "Os/Stub/test/Mutex.hpp"
#include "Os/Delegate.hpp"

namespace Os {

//! \brief get a delegate for Mutex that intercepts calls for  for stub test Mutex usage
//! \param aligned_new_memory: aligned memory to fill
//! \return: pointer to delegate
MutexInterface *MutexInterface::getDelegate(HandleStorage& aligned_placement_new_memory) {
    return Os::Delegate::makeDelegate<Mutex, Os::Stub::Mutex::Test::TestMutex>(
            aligned_placement_new_memory
    );
}
}
