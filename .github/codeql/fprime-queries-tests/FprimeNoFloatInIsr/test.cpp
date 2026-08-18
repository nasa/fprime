typedef float F32;
typedef unsigned int U32;

volatile F32 g_scaled;
volatile U32 g_count;

// Violation of cpp/fprime/no-float-in-isr: floating-point arithmetic in ISR.
void adc_isr() {
    g_scaled = g_scaled * 0.5f;
}

// Compliant: integer arithmetic only in the ISR.
void tick_isr() {
    g_count = g_count + 1;
}

// Compliant: floating-point arithmetic outside an ISR.
F32 scale(F32 value) {
    return value * 0.5f;
}
