// ======================================================================
// \title Os/Stub/DefaultQueue.cpp
// \brief sets default Os::Queue to stub implementation via linker
// ======================================================================
#include "Os/Queue.hpp"
#include "Os/Stub/Queue.hpp"
#include "Os/Delegate.hpp"

namespace Os {
QueueInterface* QueueInterface::getDelegate(QueueHandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<QueueInterface, Os::Stub::Queue::StubQueue>(aligned_new_memory);
}
}
