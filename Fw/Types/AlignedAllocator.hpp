/**
 * \file
 * \author T. Canham
 * \brief A MemAllocator implementation class that allocates aligned memory.
 *
 * \copyright
 * Copyright 2009-2021, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 *
 */

#ifndef TYPES_ALIGNEDALLOCATOR_HPP_
#define TYPES_ALIGNEDALLOCATOR_HPP_

#include <Fw/Types/MemAllocator.hpp>

namespace Fw {

    /*!
     *
     * This class is an implementation of the MemAllocator base class.
     * It uses the heap as the memory source, but aligns the allocation.
     * The allocation sizes must be a multiple of the alignment, based on the man page for aligned_alloc()
     *
     * Since it is heap space, the identifier is unused, and memory is never recoverable.
     *
     */

    class AlignedAllocator: public MemAllocator {
        public:
            AlignedAllocator(NATIVE_UINT_TYPE alignment);
            virtual ~AlignedAllocator();
            //! Allocate memory
            /*!
             * \param identifier the memory segment identifier (not used)
             * \param size the requested size (not changed)
             * \param recoverable - flag to indicate the memory could be recoverable (always set to false)
             * \return the pointer to memory. Zero if unable to allocate.
             */
            void *allocate(
                    const NATIVE_UINT_TYPE identifier,
                    NATIVE_UINT_TYPE &size,
                    bool& recoverable);
            //! Deallocate memory
            /*!
             * \param identifier the memory segment identifier (not used)
             * \param ptr the pointer to memory returned by allocate()
             */
            void deallocate(
                    const NATIVE_UINT_TYPE identifier,
                    void* ptr);

        private:
           NATIVE_UINT_TYPE m_alignment;
    };

} /* namespace Fw */

#endif /* TYPES_ALIGNEDALLOCATOR_HPP_ */
