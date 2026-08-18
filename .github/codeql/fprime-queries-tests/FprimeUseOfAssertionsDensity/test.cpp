namespace Fw {
void SwAssert(int line);
}
#define FW_ASSERT(cond) ((cond) ? static_cast<void>(0) : Fw::SwAssert(__LINE__))

typedef unsigned int U32;

// Violation of cpp/fprime/jpl-c/use-of-assertions-density (JPL Rule 16):
// a function of more than 10 lines with no assertion, that does not report
// errors through a status/bool return or a WARNING/FATAL event.
void longFunctionWithoutAssert(U32 input) {
    U32 a = input + 1;
    U32 b = a * 2;
    U32 c = b + 3;
    U32 d = c * 4;
    U32 e = d + 5;
    U32 f = e * 6;
    U32 g = f + 7;
    U32 h = g * 8;
    U32 i = h + 9;
    U32 j = i * 10;
    U32 k = j + 11;
    static_cast<void>(k);
}

// Compliant: a long function containing an FW_ASSERT.
void longFunctionWithAssert(U32 input) {
    FW_ASSERT(input < 100);
    U32 a = input + 1;
    U32 b = a * 2;
    U32 c = b + 3;
    U32 d = c * 4;
    U32 e = d + 5;
    U32 f = e * 6;
    U32 g = f + 7;
    U32 h = g * 8;
    U32 i = h + 9;
    U32 j = i * 10;
    U32 k = j + 11;
    static_cast<void>(k);
}
