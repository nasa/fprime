#ifndef _InterruptLock_hpp_
#define _InterruptLock_hpp_

#include <Fw/Types/BasicTypes.hpp>

namespace Os {
    class InterruptLock {
        public:
            
            InterruptLock(void); //!< Constructor
            virtual ~InterruptLock(void); //!< destructor
            
            int lock(void); //!< lock interrupts
            char unLock(void); //!< unlock interrupts
            
            POINTER_CAST getKey(void); //!< get the key, if used
        private:

            POINTER_CAST m_key; //!< storage of interrupt lock key if used
    };
}

#endif
