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
