
#include <Os/Log.hpp>
#include <Ref/Top/RefTopologyAc.hpp>
#include "Ref.hpp"
Ref::TopologyState state;
// Enable the console logging provided by Os::Log
Os::Log logger;

namespace Ref {
    void Initialize(const char *hostname, uint32_t port_number) {
        state = Ref::TopologyState(hostname, port_number);
        Ref::setup(state);
        
    }
    void Deinitialize() {

    }
    void run_one_cycle() {
        Ref::blockDrv.callIsr();
        Os::Task::delay(1000); //1Hz
    }
}