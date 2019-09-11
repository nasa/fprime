#include <Os/Mutex.hpp>
namespace Os {
	/**
	 * On baremetal, mutexs are not required as there is a single threaded
	 * engine to run on.
	 */
    Mutex::Mutex(void) {
        static U32 counter = 0;
        m_handle = counter++;
    }
    Mutex::~Mutex(void) {}
    void Mutex::lock(void) {}
    void Mutex::unLock(void) {}
}
