#ifndef _InterruptLock_hpp_
#define _InterruptLock_hpp_

#include <FpConfig.hpp>

namespace Os {
    class InterruptLock {
        public:

            InterruptLock(); //!< Constructor
            virtual ~InterruptLock(); //!< destructor

            void lock(); //!< lock interrupts
            void unLock(); //!< unlock interrupts

            POINTER_CAST getKey(); //!< get the key, if used
        private:

            POINTER_CAST m_key; //!< storage of interrupt lock key if used
    };
}

#endif
