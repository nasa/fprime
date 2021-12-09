#include <getopt.h>
#include <cstdlib>
#include <ctype.h>

#include <Os/Log.hpp>
#include <RPI/Top/RPITopologyAc.hpp>

#include <signal.h>
#include <cstdio>

RPI::TopologyState state;
// Enable the console logging provided by Os::Log
Os::Log logger;

void print_usage(const char* app) {
    (void) printf("Usage: ./%s [options]\n-p\tport_number\n-a\thostname/IP address\n",app);
}

static void sighandler(int signum) {
    RPI::teardown(state);
    RPI::linuxTimer.quit();
}

int main(int argc, char* argv[]) {
    I32 option = 0;

    while ((option = getopt(argc, argv, "hp:a:")) != -1){
        switch(option) {
            case 'h':
                print_usage(argv[0]);
                return 0;
                break;
            case 'p':
                state.portNumber = static_cast<U32>(atoi(optarg));
                break;
            case 'a':
                state.hostName = optarg;
                break;
            case '?':
                return 1;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    (void) printf("Hit Ctrl-C to quit\n");
    RPI::setup(state);

    // register signal handlers to exit program
    signal(SIGINT,sighandler);
    signal(SIGTERM,sighandler);

    RPI::linuxTimer.startTimer(100); //!< 10Hz

    // Give time for threads to exit
    (void) printf("Waiting for threads...\n");
    Os::Task::delay(1000);

    (void) printf("Exiting...\n");

    return 0;
}
