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
#include <stdlib.h>

namespace Fw {

    MallocAllocator::MallocAllocator() {
    }

    MallocAllocator::~MallocAllocator() {
    }

    void *MallocAllocator::allocate(NATIVE_UINT_TYPE identifier, NATIVE_UINT_TYPE size) {
        return ::malloc(size);
    }

    void MallocAllocator::deallocate(NATIVE_UINT_TYPE identifier, void* ptr) {
        return ::free(ptr);
    }

} /* namespace Fw */
