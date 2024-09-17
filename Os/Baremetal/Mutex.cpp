#include <Os/Mutex.hpp>
namespace Os {
    /**
     * On baremetal, mutexes are not required as there is a single threaded
     * engine to run on.
     */
    Mutex::Mutex() :
        m_delegate(*MutexInterface::getDelegate(this->m_handle_storage))
    {

    }

    Mutex::~Mutex() {}

    void Mutex::lock() {}

    void Mutex::unLock() {}

}
