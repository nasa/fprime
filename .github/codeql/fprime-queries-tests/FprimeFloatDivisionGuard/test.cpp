typedef float F32;
typedef double F64;
typedef unsigned int U32;
namespace Fw {
void SwAssert(const char* file, U32 line);
}
#define FW_ASSERT(cond, ...) ((cond) ? (void)0 : Fw::SwAssert(__FILE__, __LINE__))

const F32 EPSILON = 1.0e-6f;
const F32 SCALE = 2.0f;

F32 unguarded(F32 num, F32 den) {
    return num / den;  // Violation of cpp/fprime/float-division-guard
}

F32 guardedByCondition(F32 num, F32 den) {
    if (den < EPSILON && den > -EPSILON) {
        return 0.0f;
    }
    return num / den;  // Compliant
}

F32 guardedByAssert(F32 num, F32 den) {
    FW_ASSERT(den > EPSILON);
    return num / den;  // Compliant
}

F32 constantDivisor(F32 num) {
    return num / SCALE;  // Compliant: nonzero constant
}

F32 literalDivisor(F32 num) {
    return num / 2.0f;  // Compliant: literal
}

void compoundUnguarded(F32& acc, F64 den) {
    acc /= den;  // Violation of cpp/fprime/float-division-guard
}

U32 integerDivision(U32 a, U32 b) {
    return a / b;  // Compliant here: integer division is cpp/fprime/division-guard
}
