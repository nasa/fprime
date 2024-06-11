// ----------------------------------------------------------------------
// Main.cpp
// ----------------------------------------------------------------------

#include "LinuxGpioDriverTester.hpp"
#include <cstdlib>

// TEST(Test, NominalTlm) {
//   Svc::LinuxGpioDriverTester tester;
//   tester.nominalTlm();
// }

void usage(char* prog) {
    printf("Usage: %s <gpio> <mode, 0=input, 1=output, 2=interrupt>\n",prog);
}

int main(int argc, char **argv) {

    if (argc != 3) {
        usage(argv[0]);
        return -1;
    }

    Drv::LinuxGpioDriverTester tester;

    int gpio = atoi(argv[1]);
    int output = atoi(argv[2]);

    if (0 == output) {
        printf("Testing GPIO %d input\n",gpio);
        tester.testInput(gpio,10);
    }
    else if (1 == output){
        printf("Testing GPIO %d output\n",gpio);
        tester.testOutput(gpio,10);
    } else if (2 == output) {
        printf("Testing GPIO %d interrupts\n",gpio);
        tester.testInterrupt(gpio,10);
    } else {
        usage(argv[0]);
    }

}
