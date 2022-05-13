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

#define PLATFORM_INT_TYPE_DEFINED
typedef int32_t PLATFORM_INT_TYPE;
#define PRIfwpit "d"
#define PLATFORM_UINT_TYPE_DEFINED
typedef uint32_t PLATFORM_UINT_TYPE;
#define PRIfwpuit "u"

#endif
