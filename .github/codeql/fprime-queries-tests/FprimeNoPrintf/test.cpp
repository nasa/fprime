extern "C" int printf(const char* format, ...);
extern "C" int snprintf(char* str, unsigned long size, const char* format, ...);

// Violation of cpp/fprime/no-printf: printf-family calls.
void debugOutput(int value) {
    printf("value = %d\n", value);
}

// Violation: snprintf is part of the printf family.
void formatBuffer(char* buffer, unsigned long size, int value) {
    (void)snprintf(buffer, size, "%d", value);
}

// Compliant: no printf-family usage.
int compute(int value) {
    return value + 1;
}
