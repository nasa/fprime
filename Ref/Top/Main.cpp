#include <unistd.h>
#include <signal.h>
#include "Ref.hpp"

void print_usage(const char* app) {
    (void) printf("Usage: ./%s [options]\n-p\tport_number\n-a\thostname/IP address\n",app);
}

volatile sig_atomic_t terminate = 0;

static void sighandler(int signum) {
    Ref::Deinitialize();
    terminate = 1;
}

void runcycles(int cycles) {
    if (cycles == -1) {
        while (true) {
            Ref::run_one_cycle();
        }
    }

    for (int cycle = 0; cycle < cycles; cycle++) {
        Ref::run_one_cycle();
    }
}

int main(int argc, char* argv[]) {
    uint32_t port_number = 0; // Invalid port number forced
    int option;
    char *hostname;
    option = 0;
    hostname = nullptr;

    while ((option = getopt(argc, argv, "hp:a:")) != -1){
        switch(option) {
            case 'h':
                print_usage(argv[0]);
                return 0;
                break;
            case 'p':
                port_number = static_cast<uint32_t>(atoi(optarg));
                break;
            case 'a':
                hostname = optarg;
                break;
            case '?':
                return 1;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    (void) printf("Hit Ctrl-C to quit\n");

    Ref::Initialize(hostname, port_number);

    // register signal handlers to exit program
    signal(SIGINT,sighandler);
    signal(SIGTERM,sighandler);

    int cycle = 0;

    while (!terminate) {
//        (void) printf("Cycle %d\n",cycle);
        runcycles(1);
        cycle++;
    }

    // Give time for threads to exit
    (void) printf("Waiting for threads...\n");
    sleep(1);
    (void) printf("Exiting...\n");

    return 0;
}
