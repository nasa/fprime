/**
 * \file
 * \author Gene Merewether
 * \brief A MemAllocator implementation class that uses mmap.
 *
 * \copyright
 * Copyright 2009-2016, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 *
 */

#ifndef TYPES_MMAPALLOCATOR_HPP_
#define TYPES_MMAPALLOCATOR_HPP_

#include <sys/mman.h>
#include <unistd.h>
#include <Fw/DataStructures/RedBlackTreeMap.hpp>
#include <Fw/Logger/Logger.hpp>
#include <Fw/Types/MemAllocator.hpp>
#include <cerrno>

namespace Fw {

//! Fw::MmapAllocator is an implementation of the Fw::MemAllocator interface that backs memory with a read and write
//! capable anonymous memory mapped region.
//!
//! \param C - Number of allocations supported by this allocator.
//!            munmap needs the size of the original mapping. MmapAllocator keeps these sizes in a Map structure
//!            with a compile time limit on allocations
//!
template <FwSizeType C>
class MmapAllocator : public MemAllocator {
  public:
    //! Constructor with one default argument
    //!
    explicit MmapAllocator(int mmap_flags = MAP_PRIVATE | MAP_ANONYMOUS);
    //! Destructor with no arguments
    virtual ~MmapAllocator();

    //! Allocate memory using the mmap allocator
    //! \param identifier: identifier to use with allocation
    //! \param size: size of memory to be allocated
    //! \param recoverable: (output) is this memory recoverable after a reset. Always false for mmap.
    //! \param alignment - alignment requirement for the allocation. Default: maximum alignment defined by C++.
    //! \return the pointer to memory. Zero if unable to allocate
    void* allocate(const FwEnumStoreType identifier,
                   FwSizeType& size,
                   bool& recoverable,
                   FwSizeType alignment = alignof(std::max_align_t)) override;

    //! Deallocation of memory using the mmap allocator
    //! \param identifier: identifier used at allocation
    //! \param ptr: pointer to memory being deallocated
    void deallocate(const FwEnumStoreType identifier, void* ptr) override;

  private:
    //! Store sizes of previous allocations to use with deallocate
    RedBlackTreeMap<intptr_t, size_t, C> m_size_map;

    //! Store flags to pass to mmap
    const int m_mmap_flags;
};

template <FwSizeType C>
MmapAllocator<C>::MmapAllocator(int mmap_flags) : m_size_map(), m_mmap_flags(mmap_flags) {}

template <FwSizeType C>
MmapAllocator<C>::~MmapAllocator() {}

template <FwSizeType C>
void* MmapAllocator<C>::allocate(const FwEnumStoreType identifier,
                                 FwSizeType& size,
                                 bool& recoverable,
                                 FwSizeType alignment) {
    // mmap memory is never recoverable
    recoverable = false;

    // mmap allocates pages at a virtual boundary. Alignment is guaranteed
    // up to the page size
    if (alignment > static_cast<FwSizeType>(sysconf(_SC_PAGESIZE))) {
        Fw::Logger::log("Unable to allocate. Alignment request (%ld) > PAGE_SIZE (%ld)\n", alignment,
                        sysconf(_SC_PAGESIZE));
        size = 0;
        return nullptr;
    }

    if (m_size_map.getCapacity() == m_size_map.getSize()) {
        Fw::Logger::log("Unable to allocate. MmapAllocator out of size_map slots\n");
        size = 0;
        return nullptr;
    }

    void* addr = mmap(nullptr, size, PROT_READ | PROT_WRITE, m_mmap_flags, -1, 0);
    if (addr == MAP_FAILED) {
        Fw::Logger::log("Unable to allocate. mmap syscall failed. Errno (%ld)\n", errno);
        size = 0;
        return nullptr;
    }

    Fw::Success ok = m_size_map.insert(reinterpret_cast<intptr_t>(addr), size);
    FW_ASSERT(ok == Fw::Success::SUCCESS, ok);

    return addr;
}

template <FwSizeType C>
void MmapAllocator<C>::deallocate(const FwEnumStoreType identifier, void* ptr) {
    size_t alloc_size = 0;
    Fw::Success ok = m_size_map.remove(reinterpret_cast<intptr_t>(ptr), alloc_size);
    FW_ASSERT(ok == Fw::Success::SUCCESS, ok);

    int stat = munmap(ptr, alloc_size);
    FW_ASSERT(stat == 0, stat);
}

} /* namespace Fw */

#endif /* TYPES_MMAPALLOCATOR_HPP_ */
