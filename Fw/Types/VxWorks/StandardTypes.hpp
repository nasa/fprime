#ifndef VXWORKS_STD_TYPES_H
#define VXWORKS_STD_TYPES_H

#include <vxWorks.h>
#include <inttypes.h>

// Covert VxWorks OK and ERROR macros to enums
enum {
  VXWORKS_OK = OK,
  VXWORKS_ERROR = ERROR
};
#undef OK
#undef ERROR
enum {
  OK = VXWORKS_OK,
  ERROR = VXWORKS_ERROR
};

#endif
