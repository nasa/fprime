namespace Fw {
void SwAssert(int line);
}
#define FW_ASSERT(cond) ((cond) ? static_cast<void>(0) : Fw::SwAssert(__LINE__))

enum { NEVER = 0 };

typedef unsigned int U32;

// Violation of cpp/fprime/jpl-c/use-of-assertions-constant (JPL Rule 16):
// an assertion that is always true.
void assertAlwaysTrue() {
    FW_ASSERT(1 == 1);
}

// Violation: a non-literal expression that folds to zero is always false.
void assertAlwaysFalse() {
    FW_ASSERT(NEVER == 1);
}

// Compliant: FW_ASSERT(false) is the allowed unreachable-code idiom.
void assertUnreachable() {
    FW_ASSERT(false);
}

// Compliant: a dynamic assertion.
void assertDynamic(U32 value) {
    FW_ASSERT(value < 10);
}
