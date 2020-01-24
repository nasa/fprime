#include <getopt.h>
#include <stdlib.h>
#include <ctype.h>

#include <GpsApp/Top/Components.hpp>

void print_usage() {
    (void) printf("Usage: ./GpsApp -a <ground host IP address/hostname> [-p <ground port>] [-d <GPS uart device>]\n");
}

#include <signal.h>
#include <stdio.h>

volatile sig_atomic_t terminate = 0;

static void sighandler(int signum) {
    exitTasks();
    terminate = 1;
}

int main(int argc, char* argv[]) {
    I32 option = 0;
    U32 port_number = 50000;
    char* hostname = NULL;
    char* device = (char*)"/dev/ttyUSB0";

    while ((option = getopt(argc, argv, "hp:a:d:")) != -1){
        switch(option) {
            case 'h':
                print_usage();
                return 0;
                break;
            case 'p':
                port_number = atoi(optarg);
                break;
            case 'a':
                hostname = optarg;
                break;
            case 'd':
                device = optarg;
                break;
            case '?':
                return 1;
            default:
                print_usage();
                return 1;
        }
    }

    (void) printf("Hit Ctrl-C to quit\n");

    constructApp(port_number, hostname, device);

    // register signal handlers to exit program
    signal(SIGINT,sighandler);
    signal(SIGTERM,sighandler);

    while(!terminate) {
        //GPS-- Given the application's lack of a specific timing element, we
        //      force a call to the rate group driver every second here.
        //      More complex applications may drive this from a system oscillator.
        Svc::TimerVal timer;
        timer.take();
        rateGroupDriverComp.get_CycleIn_InputPort(0)->invoke(timer);
        Os::Task::TaskStatus delayStat = Os::Task::delay(1000);
        FW_ASSERT(Os::Task::TASK_OK == delayStat,delayStat);
    }
    // Give time for threads to exit
    (void) printf("Waiting for threads...\n");
    Os::Task::delay(1000);
    (void) printf("Exiting...\n");

    return 0;
}
