// File: TaskIdRepr.hpp
// Author: Nathan Serafin (nathan.serafin@jpl.nasa.gov)
// Date: 29 June, 2018
//
// Depending on the target operating system, define a type
// for the storage of task IDs.

#ifndef TASKIDREPR_HPP
#define TASKIDREPR_HPP
namespace Os {
    typedef pthread_t TaskIdRepr;
}
#endif
