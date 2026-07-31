typedef unsigned int U32;
typedef int NATIVE_INT_TYPE;

extern "C" unsigned int sleep(unsigned int seconds);

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
    void otherWork();

  private:
    Os::Mutex m_mutex;
};

// Violation of cpp/fprime/no-blocking-in-sched-handler: blocking calls in a
// rate-group handler.
void Component::schedIn_handler(NATIVE_INT_TYPE portNum, U32 context) {
    this->m_mutex.lock();
    sleep(1);
    this->m_mutex.unLock();
}

// Compliant: blocking call outside a schedIn handler.
void Component::otherWork() {
    sleep(1);
}
