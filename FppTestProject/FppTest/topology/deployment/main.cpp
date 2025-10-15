// ======================================================================
// \title  main.cpp
// \author bocchino
// \brief  Main program for FppTest deployment
// ======================================================================

#include <signal.h>
#include <cstdlib>

#include "FppTest/topology/deployment/topology/FppTestTopologyAc.hpp"
#include "Fw/Logger/Logger.hpp"
#include "Os/Os.hpp"

volatile sig_atomic_t terminate = 0;

static void signalHandler(int signum) {
    terminate = 1;
}

static const Fw::TimeInterval oneSecond(1, 0);

int main(int argc, char* argv[]) {
    Os::init();

    FppTest::TopologyState state;
    FppTest::setup(state);

    // Register signal handlers to exit program
    signal(SIGABRT, signalHandler);
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // Poll for terminate flag at 1 Hz
    U32 cycle = 0;
    while (!terminate) {
        Fw::Logger::log("cycle %" PRI_U32 "\n", cycle);
        FppTest::a1.sendData(10 * cycle);
        FppTest::a2.sendData(10 * cycle + 1);
        cycle++;
        Os::Task::delay(oneSecond);
    }

    // Tear down topology
    FppTest::teardown(state);
    // Give time for threads to exit
    Os::Task::delay(oneSecond);

    return 0;
}
