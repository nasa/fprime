/**
 * \file
 * \author T. Canham
 * \brief Implementation of malloc based allocator
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

#include <Fw/Types/MallocAllocator.hpp>
#include <stdlib.h>

namespace Fw {

    MallocAllocator::MallocAllocator() {
    }

    MallocAllocator::~MallocAllocator() {
    }

    void *MallocAllocator::allocate(NATIVE_UINT_TYPE identifier, NATIVE_UINT_TYPE size) {
        return ::malloc(size);
    }

    void MallocAllocator::deallocate(NATIVE_UINT_TYPE identifier, void* ptr) {
        return ::free(ptr);
    }

} /* namespace Fw */
