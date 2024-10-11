// ======================================================================
// \title Os/Stub/test/DefaultMutex.cpp
// \brief sets default Os::Mutex to test stub implementation via linker
// ======================================================================
#include "Os/Stub/test/Mutex.hpp"
#include "Os/Stub/test/ConditionVariable.hpp"
#include "Os/Delegate.hpp"

namespace Os {

//! \brief get a delegate for Mutex that intercepts calls for  for stub test Mutex usage
//! \param aligned_new_memory: aligned memory to fill
//! \return: pointer to delegate
MutexInterface *MutexInterface::getDelegate(MutexHandleStorage& aligned_placement_new_memory) {
    return Os::Delegate::makeDelegate<MutexInterface, Os::Stub::Mutex::Test::TestMutex>(
            aligned_placement_new_memory
    );
}

//! \brief get a delegate for MutexInterface that intercepts calls for Posix
//! \param aligned_new_memory: aligned memory to fill
//! \return: pointer to delegate
ConditionVariableInterface *ConditionVariableInterface::getDelegate(ConditionVariableHandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<ConditionVariableInterface, Os::Stub::ConditionVariable::Test::TestConditionVariable, ConditionVariableHandleStorage>(
        aligned_new_memory
    );
}
}
