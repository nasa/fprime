// ======================================================================
// \title Os/Stub/DefaultMutex.cpp
// \brief sets default Os::Mutex to no-op stub implementation via linker
// ======================================================================
#include "Os/Stub/Mutex.hpp"
#include "Os/Stub/ConditionVariable.hpp"
#include "Os/Delegate.hpp"
namespace Os {

//! \brief get a delegate for MutexInterface that intercepts calls for stub file usage
//! \param aligned_new_memory: aligned memory to fill
//! \param to_copy: pointer to copy-constructor input
//! \return: pointer to delegate
MutexInterface *MutexInterface::getDelegate(MutexHandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<MutexInterface, Os::Stub::Mutex::StubMutex>(
            aligned_new_memory
    );
}

//! \brief get a delegate for condition variable
//! \param aligned_new_memory: aligned memory to fill
//! \return: pointer to delegate
ConditionVariableInterface *ConditionVariableInterface::getDelegate(ConditionVariableHandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<ConditionVariableInterface, Os::Stub::Mutex::StubConditionVariable, ConditionVariableHandleStorage>(
        aligned_new_memory
    );
}
}
