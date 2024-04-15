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

/*!
 *
 * This class is an implementation of the MemAllocator base class.
 * It uses the heap as the memory source.
 *
 * Since it is heap space, the identifier is unused, and memory is never recoverable.
 *
 */

class MallocAllocator : public MemAllocator {
  public:
    MallocAllocator();
    virtual ~MallocAllocator();
    //! Allocate memory
    /*!
     * \param identifier the memory segment identifier (not used)
     * \param size the requested size (not changed)
     * \param recoverable - flag to indicate the memory could be recoverable (always set to false)
     * \return the pointer to memory. Zero if unable to allocate.
     */
    void* allocate(const NATIVE_UINT_TYPE identifier, NATIVE_UINT_TYPE& size, bool& recoverable);
    //! Deallocate memory
    /*!
     * \param identifier the memory segment identifier (not used)
     * \param ptr the pointer to memory returned by allocate()
     */
    void deallocate(const NATIVE_UINT_TYPE identifier, void* ptr);
};

} /* namespace Fw */

#endif /* TYPES_MALLOCALLOCATOR_HPP_ */
