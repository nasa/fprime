#include <Fw/Obj/SimpleObjRegistry.hpp>
#include <Autocoders/Python/test/port_nogen/ExampleComponentImpl.hpp>

int main(int argc, char* argv[]) {

#if FW_PORT_TRACING
    Fw::PortBase::setTrace(true);
#endif

#if FW_OBJECT_REGISTRATION == 1
    Fw::SimpleObjRegistry objReg;
#endif

    ExampleComponents::ExampleComponentImpl impl("impl");
    impl.init(10);

#if FW_OBJECT_REGISTRATION == 1
    objReg.dump();
#endif

    // This is empty just to get it to link to check for missing symbols

}
