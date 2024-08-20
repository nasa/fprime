#include <cstdio>
#include <cstdlib>
#include <ctype.h>
#include <getopt.h>
#include <signal.h>
#include <Os/Os.hpp>
#include <Fw/Time/Time.hpp>
#include <RPI/Top/RPITopologyAc.hpp>

RPI::TopologyState state;

void print_usage(const char* app) {
    (void) printf("Usage: ./%s [options]\n-p\tport_number\n-a\thostname/IP address\n",app);
}

// Handle a signal, e.g. control-C
static void sighandler(int signum) {
    // Call the teardown function
    // This causes the Linux timer to quit
    RPI::teardown(state);
}

int main(int argc, char* argv[]) {
    Os::init();
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

    // Start the Linux timer.
    // The timer runs on the main thread until it quits
    // in the teardown function, called from the signal
    // handler.
    RPI::linuxTimer.startTimer(100); //!< 10Hz

    // Signal handler was called, and linuxTimer quit.
    // Time to exit the program.
    // Give time for threads to exit.
    (void) printf("Waiting for threads...\n");
    Os::Task::delay(Fw::Time(1, 0));

    (void) printf("Exiting...\n");

    return 0;
}
