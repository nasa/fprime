#include <Autocoders/Python/test/queued1/test/ut/ComponentTesterImpl.hpp>
#include <Autocoders/Python/test/queued1/TestComponentImpl.hpp>

int main(int argc, char* argv[]) {

    SvcTest::TestComponentImpl impl("impl");
    SvcTest::ComponentTesterImpl tester("tester");
    impl.init(10);
    tester.init();

    tester.set_aport_OutputPort(0,impl.get_aport_InputPort(0));
    tester.set_aport2_OutputPort(0,impl.get_aport2_InputPort(0));

    tester.runTest();

    return 0;
}
