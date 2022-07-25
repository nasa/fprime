#include <Fw/Types/StringUtils.hpp>
#include <Tester.hpp>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

TEST(TestNominal,Nominal) {

    Drv::Tester tester;

}

const char* help = "[-h] -d <I2C device> -a <I2C address> <byte 0> <byte1> ... <byteN>";

int main(int argc, char* argv[]) {

    int c;

    U32 addr = 0;
    char device[80];
    device[0] = 0;

    while ((c = getopt (argc, argv, "hd:a:")) != -1) {
        switch (c) {
            case 'h':
                printf("test_ut %s\n",argv[0],help);
                return 0;
            case 'a':
                addr = strtoul(optarg,0,0);
                break;
            case 'd':
                Fw::StringUtils::string_copy(device, optarg, sizeof(device));
                break;
            default:
                printf("test_ut %s\n",argv[0],help);
                return -1;
        }
    }

    printf("Address: %d (0x%02X) Device: %s\n",addr,addr,device);

    U8 data[12];

    for (int i = optind; i < argc; i++) {
    	data[optind-i] = strtoul(argv[i],0,0);
        printf("Data: %s 0x%02X\n",argv[i],data[optind-i]);
    }

    Drv::Tester tester;
    tester.open(device);

    tester.sendData(addr,data,argc-optind);

    return 0;
}


