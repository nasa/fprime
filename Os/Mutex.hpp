#ifndef _Mutex_hpp_
#define _Mutex_hpp_

#include <Fw/Types/BasicTypes.hpp>

namespace Os {

    class Mutex {
        public:

            Mutex(); //!<  Constructor. Mutex is unlocked when created
            virtual ~Mutex(); //!<  Destructor

            void lock(); //!<  lock the mutex
            void unLock(); //!<  unlock the mutex

        private:

            POINTER_CAST m_handle; //!<  Stored handle to mutex
    };
}

#endif
