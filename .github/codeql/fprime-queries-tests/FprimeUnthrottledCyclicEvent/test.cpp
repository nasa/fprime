typedef unsigned int U32;
typedef int NATIVE_INT_TYPE;

class ComponentBase {
  public:
    void log_WARNING_HI_Overrun(U32 cycles) {}
    void log_WARNING_LO_Slow(U32 cycles) {
        if (this->m_SlowThrottle >= 10) {
            return;
        }
        this->m_SlowThrottle++;
    }
    void log_WARNING_LO_Slow_ThrottleClear() { this->m_SlowThrottle = 0; }
    void log_ACTIVITY_LO_Started() {}
    void log_ACTIVITY_HI_Changed(U32 v) {}
    void tlmWrite_Count(U32 v) {}

  protected:
    U32 m_SlowThrottle;
};

class Component : public ComponentBase {
  public:
    void schedIn_handler(NATIVE_INT_TYPE portNum, U32 context);
    void run_handler(NATIVE_INT_TYPE portNum, U32 context);
    void start_cmdHandler(U32 opCode, U32 cmdSeq);

  private:
    U32 m_lastValue;
    U32 read();
};

void Component::schedIn_handler(NATIVE_INT_TYPE portNum, U32 context) {
    U32 value = this->read();
    this->tlmWrite_Count(value);          // Compliant: telemetry, not an event
    this->log_WARNING_HI_Overrun(value);  // Violation of cpp/fprime/unthrottled-cyclic-event
    this->log_WARNING_LO_Slow(value);     // Compliant: throttled in FPP
    if (value != this->m_lastValue) {
        this->log_ACTIVITY_HI_Changed(value);  // Compliant: gated on state change
        this->m_lastValue = value;
    }
    if (value > 100) {
        this->log_WARNING_HI_Overrun(value);  // Violation: gate reads no state
    }
}

void Component::run_handler(NATIVE_INT_TYPE portNum, U32 context) {
    this->log_ACTIVITY_LO_Started();  // Violation of cpp/fprime/unthrottled-cyclic-event
}

void Component::start_cmdHandler(U32 opCode, U32 cmdSeq) {
    this->log_ACTIVITY_LO_Started();  // Compliant: not a cyclic handler
}
