// File: TaskIdRepr.hpp
// Author: Nathan Serafin (nathan.serafin@jpl.nasa.gov)
// Date: 29 June, 2018
//
// Depending on the target operating system, define a type
// for the storage of task IDs.

#ifndef TASKIDREPR_HPP
#define TASKIDREPR_HPP

#if defined(TGT_OS_TYPE_LINUX) || defined(TGT_OS_TYPE_DARWIN)
extern "C" {
#include <pthread.h>
}
#endif

namespace Os {
#if defined(TGT_OS_TYPE_VXWORKS) || (FW_BAREMETAL_SCHEDULER == 1)
    typedef int TaskIdRepr;
#elif defined(TGT_OS_TYPE_LINUX) || defined(TGT_OS_TYPE_DARWIN)
    typedef pthread_t TaskIdRepr;
#endif
}

#endif
