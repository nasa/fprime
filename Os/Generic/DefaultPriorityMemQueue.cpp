// ======================================================================
// \title  Os/Generic/DefaultPriorityMemQueue.cpp
// \author B. Duckett
// \brief  cpp file sets default Os::Queue to generic priority queue implementation via linker
//
// \copyright
// Copyright 2026, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================
#include "Os/Delegate.hpp"
#include "Os/Queue.hpp"
#include "PriorityMemQueue.hpp"

namespace Os {
QueueInterface* QueueInterface::getDelegate(QueueHandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<QueueInterface, Os::Generic::PriorityMemQueue, QueueHandleStorage>(
        aligned_new_memory);
}
}  // namespace Os
