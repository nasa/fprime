// Violation of cpp/fprime/incomplete-code (marker in the comment below).
// TODO: handle the overflow case
int addOne(int value) {
    return value + 1;
}

// Violation (marker in the trailing comment below).
int scale(int value) {
    return value;  // scaling factor TBD
}

// Compliant: no incomplete-code markers.
// Adds two values together.
int add(int a, int b) {
    return a + b;
}
