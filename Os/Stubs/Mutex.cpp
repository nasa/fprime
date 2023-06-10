#include <Os/Mutex.hpp>
namespace Os {
    /**
     * On baremetal, mutexes are not required as there is a single threaded
     * engine to run on.
     */
    Mutex::Mutex() {
        static U32 counter = 0;
        m_handle = counter++;
    }
    Mutex::~Mutex() {}
    void Mutex::lock() {}
    void Mutex::unLock() {}
}
