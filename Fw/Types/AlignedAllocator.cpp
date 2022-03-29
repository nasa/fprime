/**
 * \file
 * \author T. Canham
 * \brief Implementation of aligned_alloc based allocator
 *
 * \copyright
 * Copyright 2009-2016, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 *
 */

#include <Fw/Types/AlignedAllocator.hpp>
#include <stdlib.h>

namespace Fw {

    AlignedAllocator::AlignedAllocator(NATIVE_UINT_TYPE alignment)
    : m_alignment(alignment) {
    } 

    AlignedAllocator::~AlignedAllocator() {
    }

    void *AlignedAllocator::allocate(const NATIVE_UINT_TYPE identifier, NATIVE_UINT_TYPE &size, bool& recoverable) {
        // don't use identifier
        // heap memory is never recoverable
        recoverable = false;
        void *mem = ::aligned_alloc(this->m_alignment,size);
        if (NULL == mem) {
            size = 0; // set to zero if can't get memory
        }
        return mem;
    }

    void AlignedAllocator::deallocate(const NATIVE_UINT_TYPE identifier, void* ptr) {
        ::free(ptr);
    }

} /* namespace Fw */
