#include <FpConfig.hpp>

#include <Autocoders/Python/test/stress/TestCommandImpl.hpp>
#include <Autocoders/Python/test/stress/TestCommandSourceImpl.hpp>
#include <Autocoders/Python/test/stress/TestPrmSourceImpl.hpp>
#include <Autocoders/Python/test/stress/TestTelemRecvImpl.hpp>
#include <Autocoders/Python/test/stress/TestLogRecvImpl.hpp>
#include <Autocoders/Python/test/time_tester/TestTimeImpl.hpp>
#include <Autocoders/Python/test/stress/TestPtSourceImpl.hpp>

#include <Fw/Obj/SimpleObjRegistry.hpp>

int main(int argc, char* argv[]) {

#if FW_PORT_TRACING
    Fw::PortBase::setTrace(true);
#endif

#if FW_OBJECT_REGISTRATION
    Fw::SimpleObjRegistry objReg;
#endif

    TestCommand1Impl testImpl ("TestCmdImpl");
    testImpl.init(10);
    testImpl.start();

    TestCommandSourceImpl cmdSrc ("TestCmdSource");
    cmdSrc.init();

    TestParamSourceImpl prmSrc ("TestPrmSrc");
    prmSrc.init();

    TestTelemRecvImpl tlmRecv ("TestTlmRecv");
    tlmRecv.init();

    TestTimeImpl timeSource ("TimeComp");
    timeSource.init();

    TestLogRecvImpl logRecv ("TestLogRecv");
    logRecv.init();

    TestPtSourceImpl portTest ("PortTest");
    portTest.init();


    testImpl.set_CmdStatus_OutputPort(0,
            cmdSrc.get_cmdStatusPort_InputPort(0));
    testImpl.set_CmdReg_OutputPort(0,cmdSrc.get_cmdRegPort_InputPort(0));
    cmdSrc.set_cmdSendPort_OutputPort(0, testImpl.get_CmdDisp_InputPort(0));
    testImpl.regCommands();

    testImpl.set_ParamGet_OutputPort(0,prmSrc.get_paramGetPort_InputPort(0));
    testImpl.set_ParamSet_OutputPort(0,prmSrc.get_paramSetPort_InputPort(0));

    testImpl.set_Tlm_OutputPort(0,tlmRecv.get_tlmRecvPort_InputPort(0));
    testImpl.set_Time_OutputPort(0,timeSource.get_timeGetPort_InputPort(0));

    testImpl.set_Log_OutputPort(0,logRecv.get_logRecvPort_InputPort(0));

    portTest.set_aport_OutputPort(0,testImpl.get_aport_InputPort(0));
    portTest.set_aport2_OutputPort(0,testImpl.get_aport2_InputPort(0));
#if FW_ENABLE_TEXT_LOGGING
    testImpl.set_LogText_OutputPort(0,
            logRecv.get_textLogRecvPort_InputPort(0));
#endif

#if FW_OBJECT_REGISTRATION == 1
    objReg.dump();
#endif
    cmdSrc.test_TEST_CMD_1(10, 7);

    Os::Task::delay(1000);

    prmSrc.setPrm(15);
    testImpl.loadParameters();
    testImpl.printParam();

    testImpl.genTlm(Ref::Gnc::Quaternion(0.1, 0.2, 0.3, 0.4));

    portTest.aport_Test(29,56.5,240);
    portTest.aport2_Test2(30,57.5,Ref::Gnc::Quaternion(0.3,0.4,0.5,0.6));

    testImpl.sendEvent(2,3.0,4);

    testImpl.exit();
    Os::Task::delay(1000);

}
