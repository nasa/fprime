
#include <Autocoders/Python/test/app1/Top.hpp>

#include <Fw/Obj/SimpleObjRegistry.hpp>
#include <iostream>
#include <string.h>

#include <Autocoders/Python/test/app1/DuckDuckImpl.hpp>

#include <unistd.h>

using namespace std;

// Registry
static Fw::SimpleObjRegistry* simpleRegPtr = 0;

// Component instance pointers
Duck::DuckImpl* hueyComp_ptr = 0;
Duck::DuckImpl* dueyComp_ptr = 0;

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

    // Instantiate the Huey and Duey components
    hueyComp_ptr   = new Duck::DuckImpl("Duck");
    dueyComp_ptr   = new Duck::DuckImpl("Duck");


    // Connect Huey to Duey
    hueyComp_ptr->set_CmdOut_OutputPort(0, dueyComp_ptr->get_ExtCmdIn_InputPort(0));

    // Connect Duey to Huey
    dueyComp_ptr->set_CmdOut_OutputPort(0, hueyComp_ptr->get_CmdIn_InputPort(0));

    // Active component startup

    // start(identifier, stack size, priority)
    hueyComp_ptr->start(0, 10 * 1024, 100);
    dueyComp_ptr->start(0, 10 * 1024, 100);

    dumparch();
}


