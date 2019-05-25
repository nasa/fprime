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

    class MallocAllocator: public MemAllocator {
        public:
            MallocAllocator();
            virtual ~MallocAllocator();
            void *allocate(NATIVE_UINT_TYPE identifier, NATIVE_UINT_TYPE size);
            void deallocate(NATIVE_UINT_TYPE identifier, void* ptr);
    };

} /* namespace Fw */

#endif /* TYPES_MALLOCALLOCATOR_HPP_ */
