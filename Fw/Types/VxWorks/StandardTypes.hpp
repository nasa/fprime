#ifndef VXWORKS_STD_TYPES_H
#define VXWORKS_STD_TYPES_H

#include <vxWorks.h>

// Covert OK macro to constexpr
constexpr I32 VXWORKS_OK = OK;
#undef OK
constexpr I32 OK = VXWORKS_OK;

// Convert ERROR macro to constexpr
constexpr I32 VXWORKS_ERROR = ERROR;
#undef ERROR
constexpr I32 ERROR = VXWORKS_ERROR;

#endif
