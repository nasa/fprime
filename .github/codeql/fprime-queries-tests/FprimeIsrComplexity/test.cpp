typedef unsigned int U32;
extern volatile U32 REG;
extern U32 g_buffer[16];
void signalTask();

// Compliant: minimal ISR
void uart_isr() {
    g_buffer[0] = REG;
    signalTask();
}

// Violation of cpp/fprime/isr-complexity: loop in ISR
void dma_isr() {
    for (U32 i = 0; i < 16; i++) {
        g_buffer[i] = REG;
    }
    signalTask();
}

// Violation of cpp/fprime/isr-complexity: long body
void timer_interrupt_handler() {
    U32 a = REG;
    U32 b = REG;
    U32 c = REG;
    U32 d = REG;
    U32 e = REG;
    U32 f = REG;
    U32 g = REG;
    U32 h = REG;
    U32 i = REG;
    U32 j = REG;
    g_buffer[0] = a;
    g_buffer[1] = b;
    g_buffer[2] = c;
    g_buffer[3] = d;
    g_buffer[4] = e;
    g_buffer[5] = f;
    g_buffer[6] = g;
    g_buffer[7] = h;
    g_buffer[8] = i;
    g_buffer[9] = j;
    g_buffer[10] = a;
    g_buffer[11] = b;
    signalTask();
}

// Compliant: not an ISR
void processAll() {
    for (U32 i = 0; i < 16; i++) {
        g_buffer[i] = 0;
    }
}
