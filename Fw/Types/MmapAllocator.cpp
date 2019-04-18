/**
 * \file
 * \author Gene Merewether
 * \brief Implementation of mmap based allocator
 *
 * \copyright
 * Copyright 2009-2016, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 *
 */

#include <Fw/Types/MmapAllocator.hpp>
#include <stdlib.h>
#include <sys/mman.h>
#include <Fw/Types/Assert.hpp>

namespace Fw {

    MmapAllocator::MmapAllocator() : m_length(0) {
    }

    MmapAllocator::~MmapAllocator() {
    }

    void *MmapAllocator::allocate(NATIVE_UINT_TYPE identifier, NATIVE_UINT_TYPE size) {
        void* addr = mmap(NULL, size, PROT_READ | PROT_WRITE,
                             MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        if (addr == MAP_FAILED) {
            return NULL;
        }
        this->m_length = size;
        return addr;
    }

    void MmapAllocator::deallocate(NATIVE_UINT_TYPE identifier, void* ptr) {
        if (this->m_length) {
            int stat = munmap(ptr, this->m_length);
            FW_ASSERT(stat == 0, stat);
        }
    }

} /* namespace Fw */
