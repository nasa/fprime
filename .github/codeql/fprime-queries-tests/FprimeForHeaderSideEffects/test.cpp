void doWork(int value);

// Violations of cpp/fprime/for-header-flow-control: side effects in the for
// header unrelated to loop flow control.
void headerSideEffects(int n) {
    int total = 0;
    for (int i = 0; i < n; i++, total += i) {
        doWork(i);
    }

    for (int i = 0; i < n; doWork(i), i++) {
    }
}

// Compliant: header only concerns flow control.
void cleanHeader(int n) {
    int total = 0;
    for (int i = 0; i < n; i++) {
        total += i;
        doWork(total);
    }
}
