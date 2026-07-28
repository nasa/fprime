typedef unsigned int U32;
typedef int NATIVE_INT_TYPE;

extern "C" long read(int fd, void* buf, unsigned long count);

namespace Os {
class Mutex {
  public:
    void lock();
    void unLock();
};
}  // namespace Os

class Component {
  public:
    void schedIn_handler(NATIVE_INT_TYPE portNum, U32 context);
    void guardedWork();
    void unguardedWork();

  private:
    Os::Mutex m_mutex;
    char m_buffer[16];
};

// Violation of cpp/fprime/no-file-access-in-critical: file access in a
// rate-group handler.
void Component::schedIn_handler(NATIVE_INT_TYPE portNum, U32 context) {
    (void)read(0, this->m_buffer, sizeof this->m_buffer);
}

// Violation: file access while holding a lock.
void Component::guardedWork() {
    this->m_mutex.lock();
    (void)read(0, this->m_buffer, sizeof this->m_buffer);
    this->m_mutex.unLock();
}

// Compliant: file access after the lock is released.
void Component::unguardedWork() {
    this->m_mutex.lock();
    this->m_buffer[0] = 0;
    this->m_mutex.unLock();
    (void)read(0, this->m_buffer, sizeof this->m_buffer);
}
