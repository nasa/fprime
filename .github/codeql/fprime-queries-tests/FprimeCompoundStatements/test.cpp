void doWork(int value);

// Violations of cpp/fprime/compound-statements: bodies without braces.
void missingBraces(int n) {
    if (n > 0)
        doWork(n);

    for (int i = 0; i < n; i++)
        doWork(i);

    while (n > 0)
        n--;
}

// Compliant: all bodies are compound statements; else-if chains permitted.
void withBraces(int n) {
    if (n > 0) {
        doWork(n);
    } else if (n < 0) {
        doWork(-n);
    } else {
        doWork(0);
    }

    for (int i = 0; i < n; i++) {
        doWork(i);
    }
}
