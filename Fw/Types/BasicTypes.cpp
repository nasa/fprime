/**
 * \file
 * \author T. Canham
 * \brief Defines ISF basic types
 *
 * \copyright
 * Copyright 2009-2016, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 *
 */

#include <Fw/Types/BasicTypes.hpp>

#ifdef __VXWORKS__ // VxWorks doesn't have strnlen
NATIVE_INT_TYPE strnlen(const char *s, NATIVE_INT_TYPE maxlen) {
    // walk buffer looking for NULL
    for (NATIVE_INT_TYPE index = 0; index < maxlen; index++) {
        if (0 == s[index]) {
            return index+1;
        }
    }
    return maxlen;
}
#endif

