typedef unsigned int U32;
typedef int NATIVE_INT_TYPE;

class Component {
  public:
    void dataIn_handler(NATIVE_INT_TYPE portNum, U32 value);
    void configIn_handler(NATIVE_INT_TYPE portNum, U32 value);

  private:
    void log_WARNING_HI_InvalidValue(U32 value);
    void process(U32 value);
};

// Violation of cpp/fprime/validation-failure-return: the failure branch
// reports a warning but execution continues.
void Component::dataIn_handler(NATIVE_INT_TYPE portNum, U32 value) {
    if (value > 100) {
        this->log_WARNING_HI_InvalidValue(value);
    }
    this->process(value);
}

// Compliant: the failure branch returns.
void Component::configIn_handler(NATIVE_INT_TYPE portNum, U32 value) {
    if (value > 100) {
        this->log_WARNING_HI_InvalidValue(value);
        return;
    }
    this->process(value);
}
