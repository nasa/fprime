
#include <Os/Log.hpp>
#include <Ref/Top/RefTopologyAc.hpp>
#include "Ref.hpp"
Ref::TopologyState state;
// Enable the console logging provided by Os::Log
Os::Log logger;

namespace Ref {
    void Initialize(const char *hostname, unsigned int port_number) {
        state = Ref::TopologyState(hostname, static_cast<U32>(port_number));
        Ref::setup(state);
    }
    void Deinitialize() {

    }
    void run_one_cycle() {
        Ref::blockDrv.callIsr();
        Os::Task::delay(1000); //1Hz
    }
}