typedef int I32;

I32 computeStatus() {
    return 1;
}

// Violation of cpp/fprime/jpl-c/checking-return-values (JPL Rule 14):
// the non-void return value is discarded.
void ignoreStatus() {
    computeStatus();
}

// Violation: the return value is stored but never checked.
void storeStatusUnchecked() {
    I32 status;
    status = computeStatus();
}

// Compliant: the return value is checked.
I32 checkStatus() {
    I32 status = computeStatus();
    if (status != 0) {
        return status;
    }
    return 0;
}
