// ======================================================================
// \title  TraceFileLoggerTester.cpp
// \author sreddy
// \brief  cpp file for TraceFileLogger component test harness implementation class
// ======================================================================

#include "TraceFileLoggerTester.hpp"
#include <Fw/Trace/TraceEntity.hpp>
#include <Fw/Trace/TraceBuffer.hpp>

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

TraceFileLoggerTester ::TraceFileLoggerTester()
    : TraceFileLoggerGTestBase("TraceFileLoggerTester", TraceFileLoggerTester::MAX_HISTORY_SIZE), component("TraceFileLogger") {
    this->initComponents();
    this->connectPorts();
}

TraceFileLoggerTester ::~TraceFileLoggerTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------
void TraceFileLoggerTester::test_startup(){
    this->component.configure("TraceFile.dat",2720000);
    this->component.filter(0xF,Svc::TraceFileLogger_Enable::ENABLE);
}
void TraceFileLoggerTester ::test_file() {
    Fw::Time timeTag;
    timeTag.getTimeBase();
    timeTag.getContext();
    timeTag.getSeconds();
    timeTag.getUSeconds();
    
    printf("Test File Writes, including exercising circular buffer fill\n");
    
    //Write to file until it circles back to the beginning into 2 entries
    int file_entries = (TEST_TRACE_FILE_SIZE_MAX / FW_TRACE_MAX_SER_SIZE)+2;
    U8 type_entry = 0; //to iterate over trace type 
    U8 buffer[11] = {0x15,0x26,0x37,0x48,0x59,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF}; //arguments to write
    Fw::TraceBuffer trace_buffer_args(buffer,sizeof(buffer));
    
    for (int i = 0; i < file_entries; i++) {
        type_entry = (type_entry+1) % 4;
        this->invoke_to_TraceBufferLogger(0,i,timeTag,static_cast<Fw::TraceCfg::TraceType::T>(type_entry),trace_buffer_args);
        this->component.doDispatch();
    
    }
    
    printf("Test Trace Type Filter\n");
    //Filter out message_dequeue and port_call
    this->sendCmd_FilterTrace(0,1,0x6,Svc::TraceFileLogger_Enable::DISABLE);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE(0, 2, 1, Fw::CmdResponse::OK);
    this->invoke_to_TraceBufferLogger(0,0xAA,timeTag,Fw::TraceCfg::TraceType::MESSAGE_QUEUE,trace_buffer_args);
    this->component.doDispatch();
    this->invoke_to_TraceBufferLogger(0,0xBB,timeTag,Fw::TraceCfg::TraceType::MESSAGE_DEQUEUE,trace_buffer_args);
    this->component.doDispatch();
    this->invoke_to_TraceBufferLogger(0,0xCC,timeTag,Fw::TraceCfg::TraceType::PORT_CALL,trace_buffer_args);
    this->component.doDispatch();
    this->invoke_to_TraceBufferLogger(0,0xDD,timeTag,Fw::TraceCfg::TraceType::USER,trace_buffer_args);
    this->component.doDispatch();

    //enable all trace type filters and ensure they're received
    //Filter out message_dequeue and port_call
    this->sendCmd_FilterTrace(0,1,0xF,Svc::TraceFileLogger_Enable::ENABLE);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE(0, 2, 1, Fw::CmdResponse::OK);
    this->invoke_to_TraceBufferLogger(0,0xAA,timeTag,Fw::TraceCfg::TraceType::MESSAGE_QUEUE,trace_buffer_args);
    this->component.doDispatch();
    this->invoke_to_TraceBufferLogger(0,0xBB,timeTag,Fw::TraceCfg::TraceType::MESSAGE_DEQUEUE,trace_buffer_args);
    this->component.doDispatch();
    this->invoke_to_TraceBufferLogger(0,0xCC,timeTag,Fw::TraceCfg::TraceType::PORT_CALL,trace_buffer_args);
    this->component.doDispatch();
    this->invoke_to_TraceBufferLogger(0,0xDD,timeTag,Fw::TraceCfg::TraceType::USER,trace_buffer_args);
    this->component.doDispatch();

}

}  // namespace Svc
