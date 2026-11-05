typedef unsigned int U32;

// clang-format off
// Violation of cpp/fprime/jpl-c/multiple-var-decls-per-line (JPL Rule 24):
// two variables declared on a single line.
void declarePair() {
    U32 first = 0, second = 1;
    static_cast<void>(first);
    static_cast<void>(second);
}
// clang-format on

// Compliant: a range-based for loop declares one variable.
U32 sumAll(const U32 (&values)[4]) {
    U32 total = 0;
    for (const U32& value : values) {
        total += value;
    }
    return total;
}
