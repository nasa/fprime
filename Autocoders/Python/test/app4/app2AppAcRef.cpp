#include <Fw/Obj/SimpleObjRegistry.hpp>
#include <string.h>

#include <Autocoders/Python/test/app4/PartitionHubPartitionHubImpl.hpp>
#include <Autocoders/Python/test/app4/DuckDuckImpl.hpp>

#include <unistd.h>

using namespace std;

// Registry
static Fw::SimpleObjRegistry* simpleRegPtr = 0;

// Component instance pointers
PartitionHub::PartitionHubImpl* hub1Comp_ptr = 0;
Duck::DuckImpl* hueyComp_ptr = 0;

extern "C" {
    void dumparch(void);
    void dumpobj(const char* objName);
}


void dumparch(void) {
    simpleRegPtr->dump();
}

void dumpobj(const char* objName) {
    simpleRegPtr->dump(objName);
}


void constructArchitecture(void) {
    Fw::PortBase::setTrace(true);

    simpleRegPtr = new Fw::SimpleObjRegistry();

    //
    hub1Comp_ptr   = new PartitionHub::PartitionHubImpl("hub1");
    hueyComp_ptr   = new Duck::DuckImpl("Huey");

    hub1Comp_ptr->init(10);
    hueyComp_ptr->init(10);

    //
    hub1Comp_ptr->setoutputPort1SerializeOutputPort(0, hueyComp_ptr->getinputPort2Msg2InputPort(0));
    //
    hub1Comp_ptr->setoutputPort2SerializeOutputPort(0, hueyComp_ptr->getinputPort3Msg1InputPort(0));
    //
    hueyComp_ptr->setoutputPort1Msg2OutputPort(0, hub1Comp_ptr->getinputPort1SerializeInputPort(0));
    //
    hueyComp_ptr->setoutputPort2Msg1OutputPort(0, hub1Comp_ptr->getinputPort2SerializeInputPort(0));

    // Active component startup
    // start(identifier, stack size, priority)
    hueyComp_ptr->start(0, 10 * 1024, 100);
    dumparch();
}


