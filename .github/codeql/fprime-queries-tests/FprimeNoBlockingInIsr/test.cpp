extern "C" unsigned int sleep(unsigned int seconds);

namespace Os {
class Mutex {
  public:
    void lock();
    void unLock();
};
}  // namespace Os

Os::Mutex isrMutex;

// Violation of cpp/fprime/no-blocking-in-isr: blocking calls in an ISR.
void timer_isr() {
    isrMutex.lock();
    sleep(1);
    isrMutex.unLock();
}

// Compliant: no blocking operations in the ISR.
volatile int g_flag;
void gpio_isr() {
    g_flag = 1;
}

// Compliant: blocking call outside an ISR.
void backgroundTask() {
    sleep(1);
}
