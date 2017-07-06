#include <Os/InterruptLock.hpp>
#include <Os/Mutex.hpp>

STATIC Os::Mutex intLockEmulator;

namespace Os {
    InterruptLock::InterruptLock() : m_key(0) {}
    InterruptLock::~InterruptLock() {}
    
    void InterruptLock::lock(void) {
        intLockEmulator.lock();
    }

    void InterruptLock::unLock(void) {
        intLockEmulator.unLock();
    }
    
        
}


