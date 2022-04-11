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
#include <Fw/Types/Assert.hpp>
#include <cstdlib>

namespace Fw {

    AlignedAllocator::AlignedAllocator(NATIVE_UINT_TYPE alignment)
    : m_alignment(alignment) {
        // according to man page, alignment of zero is undefined so assert
        FW_ASSERT(alignment);
    } 

    AlignedAllocator::~AlignedAllocator() {
    }

    void *AlignedAllocator::allocate(const NATIVE_UINT_TYPE identifier, NATIVE_UINT_TYPE &size, bool& recoverable) {

        // Verify size is multiple of alignment by aligned_alloc rule
        FW_ASSERT(size%this->m_alignment == 0,this->m_alignment,size);

        // don't use identifier
        // heap memory is never recoverable
        recoverable = false;
        void *mem = ::aligned_alloc(this->m_alignment,size);
        if (nullptr == mem) {
            size = 0; // set to zero if can't get memory
        }
        return mem;
    }

    void AlignedAllocator::deallocate(const NATIVE_UINT_TYPE identifier, void* ptr) {
        ::free(ptr);
    }

} /* namespace Fw */
