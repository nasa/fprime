namespace Fw {
void SwAssert(int line);
}
#define FW_ASSERT(cond) ((cond) ? static_cast<void>(0) : Fw::SwAssert(__LINE__))

typedef unsigned int U32;

U32 consumeToken();

// Violation of cpp/fprime/jpl-c/use-of-assertions-side-effect (JPL Rule 16):
// an assertion whose expression performs an increment.
void assertWithIncrement(U32 limit) {
    U32 counter = 0;
    FW_ASSERT(counter++ < limit);
}

// Violation: an assertion whose expression calls a side-effecting function.
void assertWithCall(U32 limit) {
    FW_ASSERT(consumeToken() < limit);
}

// Compliant: a side-effect-free assertion.
void assertPure(U32 value, U32 limit) {
    FW_ASSERT(value < limit);
}
