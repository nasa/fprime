void doWork(int value);

// Violation of cpp/fprime/switch-case-fallthrough: non-empty case falls
// through with no annotation.
void unannotatedFallthrough(int mode) {
    switch (mode) {
        case 0:
            doWork(0);
        case 1:
            doWork(1);
            break;
        default:
            break;
    }
}

// Compliant: intentional fall-through is annotated with a comment.
void annotatedFallthrough(int mode) {
    switch (mode) {
        case 0:
            doWork(0);
            // fallthrough
        case 1:
            doWork(1);
            break;
        default:
            break;
    }
}

// Compliant: every non-empty case ends in break or return.
void terminatedCases(int mode) {
    switch (mode) {
        case 0:
            doWork(0);
            break;
        case 1:
            return;
        default:
            break;
    }
}
