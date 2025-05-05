// ======================================================================
// \title Os/Posix/DefaultFile.cpp
// \brief sets default Os::Queue to generic priority queue implementation via linker
// ======================================================================
#include "Os/Delegate.hpp"
#include "Os/Generic/PriorityQueue.hpp"
#include "Os/Queue.hpp"

namespace Os {
QueueInterface* QueueInterface::getDelegate(QueueHandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<QueueInterface, Os::Generic::PriorityQueue, QueueHandleStorage>(
        aligned_new_memory);
}
}  // namespace Os
