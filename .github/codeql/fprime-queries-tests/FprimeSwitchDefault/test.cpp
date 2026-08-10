void doWork(int value);

// Violation of cpp/fprime/switch-default: no default case.
void missingDefault(int mode) {
    switch (mode) {
        case 0:
            doWork(0);
            break;
        case 1:
            doWork(1);
            break;
    }
}

// Compliant: default case present.
void hasDefault(int mode) {
    switch (mode) {
        case 0:
            doWork(0);
            break;
        default:
            doWork(1);
            break;
    }
}
