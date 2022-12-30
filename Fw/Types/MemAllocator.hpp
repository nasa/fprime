/**
 * \file
 * \author T. Canham
 * \brief Defines a base class for a memory allocator for classes.
 *
 * A memory allocator is a class that provides memory for a component.
 * This allows a user of the class to allocate memory as they choose.
 * The user writes derived classes for each of the allocator types.
 *
 * \copyright
 * Copyright 2009-2020, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 *
 */
#ifndef TYPES_MEMALLOCATOR_HPP_
#define TYPES_MEMALLOCATOR_HPP_

#include <FpConfig.hpp>

/*!
 *
 * This class is a pure virtual base class for memory allocators in Fprime.
 * The intent is to provide derived classes the get memory from different sources.
 * The base class can be passed to classes so the allocator can be selected at the
 * system level, and different allocators can be used by different components as
 * appropriate.
 *
 * The identifier can be used to look up a pre-allocated buffer by ID in an
 * embedded system. Identifiers may be used only in a single call to an invocation.
 * Some implementations of MemAllocator discard the identifier but components using
 * the MemAllocator interface should not depend on the identifier to be discarded.
 *
 * The size is the requested size of the memory. If the allocator cannot return the
 * requested amount, it should return the actual amount and users should check.
 *
 * The recoverable flag is intended to be used in embedded environments where
 * memory can survive a processor reset and data can be recovered. The component
 * using the allocator can then use the data. Any integrity checks are up to the
 * user of the memory.
 *
 */

namespace Fw {

    class MemAllocator {
        public:
            //! Allocate memory
            /*!
             * \param identifier the memory segment identifier, each identifier is to be used in once single allocation
             * \param size the requested size - changed to actual if different
             * \param recoverable - flag to indicate the memory could be recoverable
             * \return the pointer to memory. Zero if unable to allocate
             */
            virtual void *allocate(
                    const NATIVE_UINT_TYPE identifier,
                    NATIVE_UINT_TYPE &size,
                    bool& recoverable)=0;
            //! Deallocate memory
            /*!
             * \param identifier the memory segment identifier, each identifier is to be used in once single allocation
             * \param ptr the pointer to memory returned by allocate()
             */
            virtual void deallocate(
                    const NATIVE_UINT_TYPE identifier,
                    void* ptr)=0;
        protected:
            MemAllocator();
            virtual ~MemAllocator();
        private:
            MemAllocator(MemAllocator&); //!< disable
            MemAllocator(MemAllocator*); //!< disable
    };

} /* namespace Fw */

#endif /* TYPES_MEMALLOCATOR_HPP_ */
