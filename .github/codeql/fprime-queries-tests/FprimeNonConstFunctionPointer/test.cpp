// Violation of cpp/fprime/jpl-c/non-const-function-pointer (JPL Rule 29):
// a call through a non-constant function pointer.
void invokeHandler(void (*handler)()) {
    handler();
}

// Compliant: a direct call, not through a function pointer.
void reset();

void invokeDirect() {
    reset();
}
