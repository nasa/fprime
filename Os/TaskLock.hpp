// File: TaskLock.hpp
// Author: Nathan Serafin (nathan.serafin@jpl.nasa.gov)
// Date: 25 June, 2018
//
// OS-independent wrapper to lock out task switching.

#ifndef _TaskLock_hpp_
#define _TaskLock_hpp_

#include <Fw/Types/BasicTypes.hpp>

namespace Os {
    class TaskLock {
        public:
            static I32 lock(void); //!< Lock task switching
            static I32 unLock(void); //!< Unlock task switching
    };
}

#endif
