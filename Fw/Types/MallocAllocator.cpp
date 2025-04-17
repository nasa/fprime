/**
 * \file
 * \author T. Canham
 * \brief Implementation of malloc based allocator
 *
 * \copyright
 * Copyright 2009-2016, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 *
 */

#include <Fw/Types/MallocAllocator.hpp>
#include <cstdlib>

namespace Fw {

MallocAllocator::MallocAllocator() {}

MallocAllocator::~MallocAllocator() {}

void* MallocAllocator::allocate(const FwEnumStoreType identifier, FwSizeType& size, bool& recoverable) {
    // don't use identifier
    // heap memory is never recoverable
    recoverable = false;
    void* mem = ::malloc(static_cast<size_t>(size));
    if (nullptr == mem) {
        size = 0;  // set to zero if can't get memory
    }
    return mem;
}

void MallocAllocator::deallocate(const FwEnumStoreType identifier, void* ptr) {
    ::free(ptr);
}

} /* namespace Fw */
