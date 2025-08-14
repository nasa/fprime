// ----------------------------------------------------------------------
// Main.cpp
// ----------------------------------------------------------------------

#include <cstdlib>
#include "LinuxSpiDriverTester.hpp"

// TEST(Test, NominalTlm) {
//   Svc::LinuxSpiDriverTester tester;
//   tester.nominalTlm();
// }

int main(int argc, char** argv) {
    Drv::LinuxSpiDriverTester tester;

    U8 buffer[argc - 1];

    // scan args for bytes

    for (int byte = 0; byte < argc - 1; byte++) {
        buffer[byte] = strtol(argv[1 + byte], 0, 0);
    }

    tester.sendBuffer(buffer, sizeof(buffer));
}
