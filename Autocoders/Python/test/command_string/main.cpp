#include <Autocoders/Python/test/command_string/TestCommandImpl.hpp>
#include <Autocoders/Python/test/command_string/TestCommandSourceImpl.hpp>
#include <Fw/Obj/SimpleObjRegistry.hpp>

int main(int argc, char* argv[]) {

#if FW_PORT_TRACING
    Fw::PortBase::setTrace(true);
#endif

#if FW_OBJECT_REGISTRATION == 1
    Fw::SimpleObjRegistry objReg;
#endif

	TestCommand1Impl testImpl("TestCmdImpl");
	testImpl.init(10);
	testImpl.start(10,10*1024,100);
	TestCommandSourceImpl cmdSrc("TestCmdSource");
	cmdSrc.init();

	testImpl.set_CmdStatus_OutputPort(0,cmdSrc.get_cmdStatusPort_InputPort(0));
    testImpl.set_CmdReg_OutputPort(0,cmdSrc.get_cmdRegPort_InputPort(0));
	cmdSrc.set_cmdSendPort_OutputPort(0,testImpl.get_CmdDisp_InputPort(0));
    testImpl.regCommands();


#if FW_OBJECT_REGISTRATION == 1
    objReg.dump();
#endif
    Fw::CmdStringArg str("A command string");
	cmdSrc.test_TEST_CMD_1(10,str,7);

	Os::Task::delay(1000);

	testImpl.exit();

}
