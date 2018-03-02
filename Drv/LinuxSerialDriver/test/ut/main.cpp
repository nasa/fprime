// ----------------------------------------------------------------------
// Main.cpp
// ----------------------------------------------------------------------

#include "Tester.hpp"
#include <stdlib.h>
#include "Drv/LinuxSerialDriver/LinuxSerialDriverComponentImpl.hpp"
#include <unistd.h>


//TEST(Test, NominalTlm) {
//  Svc::Tester tester;
      // Allocate read buffer
//  tester.nominalTlm();
//}

void run_simple_test(Drv::Tester& tester)
{
    BYTE pattern[] = {0xA5, 0xA5, 0xA5, 0xA5};

    tester.sendSerial(pattern,FW_NUM_ARRAY_ELEMENTS(pattern));

    // delay for 10 sec waiting for some receive data
    printf ("Delaying 10 sec for UART input\n");
    Os::Task::delay(1000);
}

void run_ramping_pattern_test(Drv::Tester& tester)
{
    BYTE pattern[256];
    for (U32 i = 0; i < FW_NUM_ARRAY_ELEMENTS(pattern); ++i) {
        pattern[i] = i;
    }

    for (NATIVE_INT_TYPE iter = 0; iter < 20*60*100; iter++) {
        tester.sendSerial(pattern,FW_NUM_ARRAY_ELEMENTS(pattern));
        Os::Task::delay(1);
    }
}

void run_router_test(Drv::Tester& tester)
{

    // Fake FcRouter packet:
    const int FC_ROUTER_PKT_SIZE = 75;
    BYTE pattern[FC_ROUTER_PKT_SIZE];
    memset(pattern,0,sizeof(pattern));
    pattern[0] = 0xa5;
    pattern[1] = 0x47;
    pattern[2] = 0x1;
    pattern[3] = 0x45;

    pattern[FC_ROUTER_PKT_SIZE-2] = 0xe6;
    pattern[FC_ROUTER_PKT_SIZE-1] = 0xd5;

    for (NATIVE_INT_TYPE iter = 0; iter < 20*60*100; iter++) {
        tester.sendSerial(pattern,FW_NUM_ARRAY_ELEMENTS(pattern));
        Os::Task::delay(1);
    }
}


int main(int argc, char **argv) {
    if ((argc < 1) || (argv[1] == NULL)) {
        printf("Need arg - name of device to open\n");
        return 1;
    }

    Drv::Tester tester(argv[1],20,READ_BUFF_SIZE,
                       Drv::LinuxSerialDriverComponentImpl::NO_FLOW,
                       Drv::LinuxSerialDriverComponentImpl::PARITY_NONE);

    run_simple_test(tester);
//    run_ramping_pattern_test(tester);
//    run_router_test(tester);

}
