#include <Fw/Obj/SimpleObjRegistry.hpp>
#include <Fw/Port/FwPortBase.hpp>

#include <Autocoders/Python/test/serialize_template/ExampleComponentImpl.hpp>

template class Fw::StructSerializable<SomeUserStruct>;

int main(int argc, char* argv[]) {

#if FW_PORT_TRACING
    Fw::PortBase::setTrace(true);
#endif

#if FW_OBJECT_REGISTRATION == 1
    Fw::SimpleObjRegistry objReg;
#endif

#if FW_OBJECT_REGISTRATION == 1
    objReg.dump();
#endif

    ExampleComponentImpl comp
#if FW_OBJECT_NAMES
    ("ExComp");
#else
    ;
#endif

    comp.init(10);
    comp.start(10, 10 * 1024, 100);

    AnotherExample::InputExamplePort* port = comp.getexampleInputExampleInputPort(0);

    SomeUserStruct val = {12,29.6,2};
    Fw::StructSerializable<SomeUserStruct> arg;
    arg.set(val);

    port->invoke(-10,arg);

    Os::Task::delay(1000);

    comp.exit();

}
