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
 * acknowledged. Any commercial use must be negotiated with the Office
 * of Technology Transfer at the California Institute of Technology.
 *
 * This software may be subject to U.S. export control laws and
 * regulations.  By accepting this document, the user agrees to comply
 * with all U.S. export laws and regulations.  User has the
 * responsibility to obtain export licenses, or other export authority
 * as may be required before exporting such information to foreign
 * countries or providing access to foreign persons.
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
