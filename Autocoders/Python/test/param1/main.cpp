#include <Autocoders/Python/test/param1/TestPrmImpl.hpp>
#include <Autocoders/Python/test/param1/TestPrmSourceImpl.hpp>
#include <Fw/Obj/SimpleObjRegistry.hpp>

int main(int argc, char* argv[]) {

#if FW_PORT_TRACING
    Fw::PortBase::setTrace(true);
#endif

#if FW_OBJECT_REGISTRATION == 1
    Fw::SimpleObjRegistry objReg;
#endif

	TestPrmImpl testImpl("TestPrmImpl");
	testImpl.init();
	TestParamSourceImpl prmSrc("TestPrmSrc");
	prmSrc.init();

    testImpl.set_ParamGet_OutputPort(0,prmSrc.get_paramGetPort_InputPort(0));
    testImpl.set_ParamSet_OutputPort(0,prmSrc.get_paramSetPort_InputPort(0));
#if FW_OBJECT_REGISTRATION == 1
    objReg.dump();
#endif

    prmSrc.setPrm(15);
    testImpl.loadParameters();
	testImpl.printParam();

}
