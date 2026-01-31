/**
 * \file
 * \author T. Canham
 * \brief A MemAllocator implementation class that uses malloc.
 *
 * \copyright
 * Copyright 2009-2016, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 *
 */

#ifndef TYPES_MALLOCALLOCATOR_HPP_
#define TYPES_MALLOCALLOCATOR_HPP_

#include <Fw/Types/MemAllocator.hpp>

namespace Fw {

//! \brief malloc based memory allocator
//!
//! This class implements a memory allocator that uses malloc/free to allocate memory and deallocate memory. malloc()
//! guarantees alignment for any type and thus does this allocator. It will not respect smaller alignments.
//!
//! Since this directs to heap space, the identifier is unused and memory is never recoverable.
class MallocAllocator : public MemAllocator {
  public:
    MallocAllocator() = default;
    virtual ~MallocAllocator() = default;

    //! Allocate memory
    //!
    //! Allocate memory using malloc(). The identifier is unused and memory is never recoverable.
    //! malloc() guarantees alignment for any type and so does this allocator. It will not respect smaller alignments.
    //!
    //! \param identifier the allocating entity identifier (not used)
    //! \param size the requested size (not changed)
    //! \param recoverable - flag to indicate the memory could be recoverable (always set to false)
    //! \param alignment - alignment requirement for the allocation. Default: maximum alignment defined by C++.
    //! \return the pointer to memory. Zero if unable to allocate.
    void* allocate(const FwEnumStoreType identifier,
                   FwSizeType& size,
                   bool& recoverable,
                   FwSizeType alignment = alignof(std::max_align_t)) override;
    //! Deallocate memory
    //!
    //! Deallocate memory previously allocated by allocate() using free(). The identifier is unused but should still
    //! match the original call.
    //!
    //! \param identifier the memory segment identifier (not used)
    //! \param ptr the pointer to memory returned by allocate()
    void deallocate(const FwEnumStoreType identifier, void* ptr) override;
};

} /* namespace Fw */

#endif /* TYPES_MALLOCALLOCATOR_HPP_ */
