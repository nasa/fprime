#include <Os/InterruptLock.hpp>
#include <Os/Mutex.hpp>

STATIC Os::Mutex intLockEmulator;

namespace Os {
    InterruptLock::InterruptLock() : m_key(0) {}
    InterruptLock::~InterruptLock() {}

    void InterruptLock::lock() {
        intLockEmulator.lock();
    }

    void InterruptLock::unLock() {
        intLockEmulator.unLock();
    }


}


