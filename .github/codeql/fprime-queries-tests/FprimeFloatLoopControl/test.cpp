typedef unsigned int U32;
typedef float F32;

void doWork(F32 value);

// Violation of cpp/fprime/float-loop-control: floating-point loop control.
void floatControlled() {
    for (F32 x = 0.0f; x < 1.0f; x += 0.1f) {
        doWork(x);
    }
}

// Compliant: integer loop control.
void intControlled() {
    for (U32 i = 0; i < 10; i++) {
        doWork(static_cast<F32>(i));
    }
}
