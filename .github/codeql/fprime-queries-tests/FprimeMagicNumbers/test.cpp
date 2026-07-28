typedef unsigned int U32;

static const U32 BUFFER_SIZE = 64;

// Violation of cpp/fprime/magic-numbers: unnamed numeric literal.
U32 scaleReading(U32 raw) {
    return raw * 37;
}

// Compliant: trivial values 0 and 1 are permitted.
U32 increment(U32 value) {
    return (value == 0) ? 1 : value + 1;
}

// Compliant: literal names a constant.
U32 bufferSize() {
    return BUFFER_SIZE;
}

// Compliant: powers of 2 (up to 4096), all-ones masks, and powers of 10.
U32 idiomaticValues(U32 value) {
    U32 shifted = (value >> 8) & 0xFF;
    return (shifted * 1000) % 4096;
}

// Compliant: non-local (file-scope) definition, even when not const.
U32 g_counter = 500;

// Compliant: multiples of 8 as shift amounts; float/double multiples of 10.
double byteSwapAndScale(unsigned long long value) {
    unsigned long long high = value >> 56;
    return static_cast<double>(high) / 250.0;
}

// Violation: magic number assigned to a local variable.
U32 localAssignment() {
    U32 window = 77;
    return window;
}
