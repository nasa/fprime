typedef unsigned int U32;

void doWork(U32 value);

// Violation of cpp/fprime/jpl-c/loop-bounds (JPL Rule 3):
// a loop with no fixed upper bound.
void spinUnbounded(bool flag) {
    while (flag) {
        doWork(0);
    }
}

// Violation: the loop counter is never incremented in the body.
void stallCounter(U32 n) {
    for (U32 i = 0; i < n;) {
        doWork(i);
    }
}

// Compliant: bounded loop with the counter incremented on every path.
void boundedLoop(U32 n) {
    for (U32 i = 0; i < n; i++) {
        doWork(i);
    }
}
