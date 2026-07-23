typedef unsigned int U32;

U32 nextToken();

// Violation of cpp/fprime/jpl-c/no-boolean-side-effects (JPL Rule 19):
// an assignment performed inside a boolean expression.
void consumeTokens() {
    U32 value = 0;
    while ((value = nextToken()) != 0) {
    }
}

// Violation: an increment performed inside a boolean expression.
void countUp(U32 limit) {
    U32 counter = 0;
    if (counter++ < limit) {
        counter = 0;
    }
}

// Compliant: the boolean expression is side-effect free.
bool inRange(U32 value, U32 limit) {
    if (value < limit) {
        return true;
    }
    return false;
}
