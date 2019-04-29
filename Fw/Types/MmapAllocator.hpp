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

    class MmapAllocator: public MemAllocator {
        public:
            MmapAllocator();
            virtual ~MmapAllocator();
            void *allocate(NATIVE_UINT_TYPE identifier, NATIVE_UINT_TYPE size);
            void deallocate(NATIVE_UINT_TYPE identifier, void* ptr);

        private:
            NATIVE_UINT_TYPE m_length;
    };

} /* namespace Fw */

#endif /* TYPES_MMAPALLOCATOR_HPP_ */
