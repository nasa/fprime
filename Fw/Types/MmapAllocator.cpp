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

#include <sys/mman.h>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/MmapAllocator.hpp>
#include <cstdlib>

namespace Fw {

MmapAllocator::MmapAllocator() : m_length(0) {}

MmapAllocator::~MmapAllocator() {}

void* MmapAllocator::allocate(const NATIVE_UINT_TYPE identifier, NATIVE_UINT_TYPE& size, bool& recoverable) {
    void* addr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (addr == MAP_FAILED) {
        size = 0;
        return nullptr;
    }
    this->m_length = size;

    // mmap memory is never recoverable
    recoverable = false;

    return addr;
}

void MmapAllocator::deallocate(const NATIVE_UINT_TYPE identifier, void* ptr) {
    if (this->m_length) {
        int stat = munmap(ptr, this->m_length);
        FW_ASSERT(stat == 0, stat);
    }
}

} /* namespace Fw */
