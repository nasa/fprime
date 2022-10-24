// File: TaskLock.hpp
// Author: Nathan Serafin (nathan.serafin@jpl.nasa.gov)
// Date: 25 June, 2018
//
// OS-independent wrapper to lock out task switching.

#ifndef _TaskLock_hpp_
#define _TaskLock_hpp_

#include <FpConfig.hpp>

namespace Os {
    class TaskLock {
        public:
            static I32 lock(); //!< Lock task switching
            static I32 unLock(); //!< Unlock task switching
    };
}

#endif
