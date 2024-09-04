// ======================================================================
// \title  TraceLoggerTester.cpp
// \author sreddy
// \brief  cpp file for TraceLogger component test harness implementation class
// ======================================================================

#include "TraceLoggerTester.hpp"
#include <Fw/Trace/TracePacket.hpp>
#include <Fw/Trace/TraceBuffer.hpp>

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

TraceLoggerTester ::TraceLoggerTester()
    : TraceLoggerGTestBase("TraceLoggerTester", TraceLoggerTester::MAX_HISTORY_SIZE), component("TraceLogger") {
    this->initComponents();
    this->connectPorts();
}

TraceLoggerTester ::~TraceLoggerTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------
void TraceLoggerTester::test_startup(){
    this->component.configure("TraceFile.dat");
    this->component.filter(0xF,Svc::TraceLogger_Enable::ENABLE);
}
void TraceLoggerTester ::test_file() {
    Fw::Time timeTag;
    timeTag.getTimeBase();
    timeTag.getContext();
    timeTag.getSeconds();
    timeTag.getUSeconds();
    U8 buffer[5]= {1,2,3,4,5};
    Fw::TraceBuffer trace_buffer(buffer,sizeof(buffer));
    printf("Test File Writes, including exercising circular buffer fill\n");

    this->invoke_to_TraceBufferLogger(0,1,timeTag,Fw::TraceCfg::TraceType::MESSAGE_QUEUE,trace_buffer);
    this->component.doDispatch();
    U8 buffer_1[11] = {0x15,0x26,0x37,0x48,0x59,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF};
    Fw::TraceBuffer trace_buffer_1(buffer_1,sizeof(buffer_1));
    this->invoke_to_TraceBufferLogger(0,2,timeTag,Fw::TraceCfg::TraceType::MESSAGE_QUEUE,trace_buffer_1);
    this->component.doDispatch();
    this->invoke_to_TraceBufferLogger(0,3,timeTag,Fw::TraceCfg::TraceType::MESSAGE_DEQUEUE,trace_buffer_1);
    this->component.doDispatch();
    this->invoke_to_TraceBufferLogger(0,4,timeTag,Fw::TraceCfg::TraceType::USER,trace_buffer_1);
    this->component.doDispatch();
    this->invoke_to_TraceBufferLogger(0,5,timeTag,Fw::TraceCfg::TraceType::PORT_CALL,trace_buffer_1);
    this->component.doDispatch();
    printf("Test Trace Type Filter\n");
    this->sendCmd_FilterTrace(0,1,0x6,Svc::TraceLogger_Enable::DISABLE);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE(0, 2, 1, Fw::CmdResponse::OK);
    this->invoke_to_TraceBufferLogger(0,6,timeTag,Fw::TraceCfg::TraceType::USER,trace_buffer_1);
    this->component.doDispatch();
    this->invoke_to_TraceBufferLogger(0,7,timeTag,Fw::TraceCfg::TraceType::MESSAGE_DEQUEUE,trace_buffer_1);
    this->component.doDispatch();
    printf("EXEC last data write\n");
    this->invoke_to_TraceBufferLogger(0,8,timeTag,Fw::TraceCfg::TraceType::MESSAGE_QUEUE,trace_buffer_1);
    this->component.doDispatch();
    this->invoke_to_TraceBufferLogger(0,9,timeTag,Fw::TraceCfg::TraceType::MESSAGE_QUEUE,trace_buffer_1);
    this->component.doDispatch();
}

}  // namespace Svc
