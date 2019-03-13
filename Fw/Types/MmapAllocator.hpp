/**
 * \file
 * \author Gene Merewether
 * \brief A MemAllocator implementation class that uses mmap.
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
