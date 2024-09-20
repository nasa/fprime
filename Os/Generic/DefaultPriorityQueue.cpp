// ======================================================================
// \title Os/Posix/DefaultFile.cpp
// \brief sets default Os::File to posix implementation via linker
// ======================================================================
#include "Os/Queue.hpp"
#include "Os/Generic/PriorityQueue.hpp"
#include "Os/Delegate.hpp"

namespace Os {
QueueInterface* QueueInterface::getDelegate(QueueHandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<QueueInterface, Os::Generic::PriorityQueue, QueueHandleStorage>(aligned_new_memory);
}
}
