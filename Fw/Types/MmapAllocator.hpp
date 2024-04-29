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
    void* allocate(const NATIVE_UINT_TYPE identifier, NATIVE_UINT_TYPE& size, bool& recoverable);

    //! Deallocation of memory using the mmap allocator
    //! \param identifier: identifier used at allocation
    //! \param ptr: pointer to memory being deallocated
    void deallocate(const NATIVE_UINT_TYPE identifier, void* ptr);

  private:
    NATIVE_UINT_TYPE m_length;
};

} /* namespace Fw */

#endif /* TYPES_MMAPALLOCATOR_HPP_ */
