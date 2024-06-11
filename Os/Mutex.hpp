#ifndef _Mutex_hpp_
#define _Mutex_hpp_

#include <FpConfig.hpp>

namespace Os {

    class Mutex {
        public:

            Mutex(); //!<  Constructor. Mutex is unlocked when created
            virtual ~Mutex(); //!<  Destructor

            void lock(); //!<  lock the mutex
            void unLock(); //!<  unlock the mutex
            void unlock() { this->unLock(); } //!<  alias for unLock to meet BasicLockable requirements

        private:

            POINTER_CAST m_handle; //!<  Stored handle to mutex
    };
}

#endif
