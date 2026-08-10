typedef unsigned int U32;

// clang-format off
// Violation of cpp/fprime/jpl-c/multiple-stmts-per-line (JPL Rule 24):
// two sibling statements on a single line.
void updatePair() {
    U32 counterA = 0;
    U32 counterB = 0;
    counterA++; counterB++;
    static_cast<void>(counterA);
    static_cast<void>(counterB);
}

// Compliant: a lone nested statement on one line is not flagged.
bool isPositive(int value) { return value > 0; }
// clang-format on
