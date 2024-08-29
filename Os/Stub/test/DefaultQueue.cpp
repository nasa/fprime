// ======================================================================
// \title Os/Stub/test/DefaultQueue.cpp
// \brief sets default Os::Queue to stub test implementation via linker
// ======================================================================
#include "Os/Queue.hpp"
#include "Os/Stub/test/Queue.hpp"
#include "Os/Delegate.hpp"

namespace Os {
QueueInterface* QueueInterface::getDelegate(HandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<QueueInterface, Os::Stub::Queue::Test::InjectableStlQueue>(aligned_new_memory);
}
}
