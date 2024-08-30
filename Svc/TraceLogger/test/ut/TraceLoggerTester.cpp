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
}
void TraceLoggerTester ::test_file() {
    Fw::Time timeTag;
    timeTag.getTimeBase();
    timeTag.getContext();
    timeTag.getSeconds();
    timeTag.getUSeconds();
    U8 buffer[5]= {1,2,3,4,5};
    Fw::TraceBuffer trace_buffer(buffer,sizeof(buffer));
    printf("Invoking the port here:\n");

    this->invoke_to_TraceBufferLogger(0,1,timeTag,Fw::TraceCfg::TraceType::MESSAGE_QUEUE,trace_buffer);
    this->component.doDispatch();
    U8 buffer_1[11] = {0x15,0x26,0x37,0x48,0x59,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF};
    Fw::TraceBuffer trace_buffer_1(buffer_1,sizeof(buffer_1));
    this->invoke_to_TraceBufferLogger(0,2,timeTag,Fw::TraceCfg::TraceType::MESSAGE_QUEUE,trace_buffer_1);
    this->component.doDispatch();

}

}  // namespace Svc
