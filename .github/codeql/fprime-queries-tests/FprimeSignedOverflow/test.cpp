typedef int I32;
typedef unsigned int U32;
namespace Fw {
void SwAssert(const char* file, U32 line);
}
#define FW_ASSERT(cond, ...) ((cond) ? (void)0 : Fw::SwAssert(__FILE__, __LINE__))

const I32 LIMIT = 1000;

I32 unguardedAdd(I32 a, I32 b) {
    return a + b;  // Violation of cpp/fprime/signed-overflow
}

I32 unguardedMul(I32 a, I32 b) {
    return a * b;  // Violation of cpp/fprime/signed-overflow
}

I32 guardedByCondition(I32 a, I32 b) {
    if (a > LIMIT || a < -LIMIT || b > LIMIT || b < -LIMIT) {
        return 0;
    }
    return a * b;  // Compliant: operands range-checked
}

I32 guardedByAssert(I32 a, I32 b) {
    FW_ASSERT(a < LIMIT && a > -LIMIT);
    return a + b;  // Compliant: an operand is checked (reviewer judges sufficiency)
}

U32 unsignedAdd(U32 a, U32 b) {
    return a + b;  // Compliant: unsigned wraps by definition
}

I32 loopCounter(I32 n) {
    I32 sum = 0;
    for (I32 i = 0; i < n; i++) {
        sum = sum + 1;  // Compliant: literal operand
    }
    return sum;
}
