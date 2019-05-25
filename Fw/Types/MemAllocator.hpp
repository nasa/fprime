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
 * Copyright 2009-2016, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 *
 */
#ifndef TYPES_MEMALLOCATOR_HPP_
#define TYPES_MEMALLOCATOR_HPP_

#include <Fw/Types/BasicTypes.hpp>

namespace Fw {

    class MemAllocator {
        public:
            virtual void *allocate(NATIVE_UINT_TYPE identifier, NATIVE_UINT_TYPE size)=0;
            virtual void deallocate(NATIVE_UINT_TYPE identifier, void* ptr)=0;
        protected:
            MemAllocator();
            virtual ~MemAllocator();
        private:
            MemAllocator(MemAllocator&); // disable
            MemAllocator(MemAllocator*); // disable
    };

} /* namespace Fw */

#endif /* TYPES_MEMALLOCATOR_HPP_ */
