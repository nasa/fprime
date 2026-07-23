namespace Fw {
void SwAssert(int line);
}
#define FW_ASSERT(cond) ((cond) ? static_cast<void>(0) : Fw::SwAssert(__LINE__))

typedef unsigned int U32;

// Violation of cpp/fprime/jpl-c/use-of-assertions-non-boolean (JPL Rule 16):
// asserting a pointer instead of a boolean test.
void assertPointer(const U32* ptr) {
    FW_ASSERT(ptr);
    static_cast<void>(*ptr);
}

// Violation: asserting an integer instead of a boolean test.
void assertInteger(U32 count) {
    FW_ASSERT(count);
}

// Compliant: the assertion is a boolean test.
void assertBoolean(const U32* ptr) {
    FW_ASSERT(ptr != nullptr);
    static_cast<void>(*ptr);
}
