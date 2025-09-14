// ======================================================================
// \title Os/Stub/test/DefaultQueue.cpp
// \brief sets default Os::Queue to stub test implementation via linker
// ======================================================================
#include "Os/Delegate.hpp"
#include "Os/Queue.hpp"
#include "Os/Stub/test/Queue.hpp"

namespace Os {
QueueInterface* QueueInterface::getDelegate(QueueHandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<QueueInterface, Os::Stub::Queue::Test::InjectableStlQueue, QueueHandleStorage>(
        aligned_new_memory);
}
}  // namespace Os
