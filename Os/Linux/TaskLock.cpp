// File: TaskLock.cpp
// Author: Nathan Serafin (nathan.serafin@jpl.nasa.gov)
// Date: 27 June, 2018
//
// Dummy implementation of TaskLock::lock() and TaskLock::unLock() for Linux.

#include <Fw/Types/Assert.hpp>

#include <Os/TaskLock.hpp>

namespace Os {

    I32 TaskLock::lock(void)
    {
        FW_ASSERT(0);
        return 0;
    }

    I32 TaskLock::unLock(void)
    {
        FW_ASSERT(0);
        return 0;
    }

}

