#ifndef _Mutex_hpp_
#define _Mutex_hpp_

#include <Fw/Types/BasicTypes.hpp>

namespace Os {
    
    class Mutex {
        public:
            
            Mutex(void); //!<  Constructor. Mutex is unlocked when created
            virtual ~Mutex(void); //!<  Destructor
            
            void lock(void); //!<  lock the mutex
            void unLock(void); //!<  unlock the mutex
            
        private:

            POINTER_CAST m_handle; //!<  Stored handle to mutex
    };
}

#endif
