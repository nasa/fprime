#ifndef VXWORKS_STD_TYPES_H
#define VXWORKS_STD_TYPES_H

#include <vxWorks.h>

#if OK != 0
#error "Expected OK = 0 in VxWorks build"
#endif
#define VXWORKS_OK 0
#undef OK

#if ERROR != -1
#error "Expected ERROR = -1 in VxWorks build"
#endif
#define VXWORKS_ERROR -1
#undef ERROR

#endif
