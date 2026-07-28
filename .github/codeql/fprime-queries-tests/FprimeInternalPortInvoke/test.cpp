typedef unsigned int U32;

class Component {
  public:
    void update(U32 value);
    void doDispatch();

  protected:
    void newValue_internalInterfaceInvoke(U32 value);
    void newValue_internalInterfaceHandler(U32 value);
};

// Violation of cpp/fprime/internal-port-invoke: handler called directly,
// bypassing the component queue.
void Component::update(U32 value) {
    this->newValue_internalInterfaceHandler(value);
}

// Compliant: interactions go through the auto-coded invoke function; the
// dispatch loop is the only legitimate direct caller of the handler.
void Component::doDispatch() {
    this->newValue_internalInterfaceHandler(0);
}

void Component::newValue_internalInterfaceInvoke(U32 value) {
    // auto-coded: enqueue message for doDispatch
}
