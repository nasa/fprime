// File: TaskId.hpp
// Author: Ben Soudry (benjamin.s.soudry@jpl.nasa.gov)
//         Nathan Serafin (nathan.serafin@jpl.nasa.gov)
// Date: 29 June, 2018
//
// Define a type for task IDs.  This is useful as POSIX only
// provides an opaque TID with a special pthread_equal() comparison
// function.  For higher-level code to not need to be aware of
// POSIX versus VxWorks versus whatever else, we can overload the
// == operator to perform the correct equality check.

#ifndef _TaskId_hpp_
#define _TaskId_hpp_

#include <Os/TaskIdRepr.hpp>

namespace Os {
  class TaskId {
    public:
        TaskId();
        ~TaskId();
        bool operator==(const TaskId& T) const;
        bool operator!=(const TaskId& T) const;
        TaskIdRepr getRepr() const;
    private:
        TaskIdRepr id;
  };
}

#endif
