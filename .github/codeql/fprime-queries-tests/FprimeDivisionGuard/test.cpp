typedef unsigned int U32;

void swAssert();
#define FW_ASSERT(cond) ((cond) ? (void)0 : swAssert())

// Violation of cpp/fprime/division-guard: divisor never checked.
U32 unguarded(U32 total, U32 count) {
    return total / count;
}

// Compliant: divisor checked with FW_ASSERT before use.
U32 assertGuarded(U32 total, U32 count) {
    FW_ASSERT(count != 0);
    return total / count;
}

// Compliant: divisor checked with a condition before use.
U32 conditionGuarded(U32 total, U32 count) {
    if (count == 0) {
        return 0;
    }
    return total % count;
}

// Compliant: constant nonzero divisor.
U32 constantDivisor(U32 total) {
    return total / 4;
}
