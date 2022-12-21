// File: TaskId.cpp
// Author: Ben Soudry (benjamin.s.soudry@jpl.nasa.gov)
//         Nathan Serafin (nathan.serafin@jpl.nasa.gov)
// Date: 29 June, 2018
//
// POSIX implementation of TaskId type.

extern "C" {
#include <pthread.h>
}

#include <Os/TaskId.hpp>

namespace Os
{
    TaskId::TaskId() : id(pthread_self())
    {
    }
    TaskId::~TaskId()
    {
    }
    bool TaskId::operator==(const TaskId& T) const
    {
        return pthread_equal(id, T.id);
    }
    bool TaskId::operator!=(const TaskId& T) const
    {
        return !pthread_equal(id, T.id);
    }
    TaskIdRepr TaskId::getRepr() const
    {
        return this->id;
    }
}
