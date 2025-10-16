/**
 * \copyright
 * Copyright 2009-2016, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 */
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/MemAllocator.hpp>

namespace Fw {

MemAllocatorRegistry* MemAllocatorRegistry::s_registry = nullptr;  //!< singleton registry

MemAllocator::MemAllocator() {}

MemAllocator::~MemAllocator() {}

MemAllocatorRegistry::MemAllocatorRegistry() {
    // Register self as the singleton
    MemAllocatorRegistry::s_registry = this;
}

void MemAllocatorRegistry::registerAllocator(const MemoryAllocation::MemoryAllocatorType type,
                                             MemAllocator& allocator) {
    this->m_allocators[type] = &allocator;
}

MemAllocatorRegistry& MemAllocatorRegistry::getInstance() {
    FW_ASSERT(s_registry != nullptr);
    return *s_registry;
}

MemAllocator& MemAllocatorRegistry::getAllocator(const MemoryAllocation::MemoryAllocatorType type) {
    FW_ASSERT(this->m_allocators[type] != nullptr, static_cast<FwAssertArgType>(type));
    return *this->m_allocators[type];
}

MemAllocator& MemAllocatorRegistry::getAnAllocator(const MemoryAllocation::MemoryAllocatorType type) {
    // If the allocator is not registered, return the SYSTEM allocator
    if (this->m_allocators[type] == nullptr) {
        return this->getAllocator(MemoryAllocation::MemoryAllocatorType::SYSTEM);
    }
    return *this->m_allocators[type];
}
} /* namespace Fw */
