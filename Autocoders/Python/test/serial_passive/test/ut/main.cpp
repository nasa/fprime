#include <Autocoders/Python/test/serial_passive/TestSerialImpl.hpp>
#include <Autocoders/Python/test/port_loopback/ExampleComponentImpl.hpp>

int main(int argc, char* argv[]) {

    TestComponents::TestSerialImpl serImpl("SerImpl");
    serImpl.init(10,100);
    ExampleComponents::ExampleComponentImpl exImpl("ExImpl");
    exImpl.init();
    serImpl.start(100,100,10*1024);

    // connect ports
    exImpl.set_exampleOutput_OutputPort(0,serImpl.get_SerialInSync_InputPort(0));
    exImpl.set_exampleOutput_OutputPort(1,serImpl.get_SerialInGuarded_InputPort(0));
    exImpl.set_exampleOutput_OutputPort(2,serImpl.get_SerialInAsync_InputPort(0));
    serImpl.set_SerialOut_OutputPort(0,exImpl.get_exampleInput_InputPort(0));

    exImpl.makePortCall(0);
    exImpl.makePortCall(1);
    exImpl.makePortCall(2);

    Os::Task::delay(500);
    serImpl.exit();

    return 0;

}
