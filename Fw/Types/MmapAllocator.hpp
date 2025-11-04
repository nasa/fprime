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

#include <Fw/Types/MemAllocator.hpp>

namespace Fw {

//! Fw::MmapAllocator is an implementation of the Fw::MemAllocator interface that back memory with a read and write
//! capable anonymous memory mapped region. This class is currently not useful for mapping to a file.
class MmapAllocator : public MemAllocator {
  public:
    //! Constructor with no arguments
    //!
    MmapAllocator();
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
    FwSizeType m_length;
};

} /* namespace Fw */

#endif /* TYPES_MMAPALLOCATOR_HPP_ */
