/**
 * \file
 * \author T. Canham
 * \brief A MemAllocator implementation class that uses malloc.
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
