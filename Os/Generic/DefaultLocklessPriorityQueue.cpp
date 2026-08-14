// ======================================================================
// \title Os/Generic/DefaultLocklessPriorityQueue.cpp
// \brief sets default Os::Queue to the lockless priority queue implementation via linker
// ======================================================================
#include "Os/Delegate.hpp"
#include "Os/Generic/LocklessPriorityQueue.hpp"
#include "Os/Queue.hpp"

namespace Os {
QueueInterface* QueueInterface::getDelegate(QueueHandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<QueueInterface, Os::Generic::LocklessPriorityQueue, QueueHandleStorage>(
        aligned_new_memory);
}
}  // namespace Os
