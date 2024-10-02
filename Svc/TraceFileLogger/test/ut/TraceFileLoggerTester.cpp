// ======================================================================
// \title  TraceFileLoggerTester.cpp
// \author sreddy
// \brief  cpp file for TraceFileLogger component test harness implementation class
// ======================================================================

#include "TraceFileLoggerTester.hpp"
#include <Fw/Trace/TraceEntity.hpp>
#include <Fw/Trace/TraceBuffer.hpp>
#include <fstream>
#include <iostream>
#include <cstring>
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
    this->component.configure("TraceFileTest.dat",TEST_TRACE_FILE_SIZE_MAX);
    this->component.filterTraceType(0xF,Svc::TraceFileLogger_Enable::ENABLE);
}

void TraceFileLoggerTester ::test_file() {
    Fw::Time timeTag;
    U8 trace_id;
    U16 time_store_type;
    //U8 time_context_type;
    U8 time_secs;
    U8 time_usecs;
    U8 arg_size;
    std::array<U8,11> arg_check;
    U8* arg_check_ptr = &arg_check[0];
    
    printf("Test File Writes, including exercising circular buffer fill\n");
    //Write to file until it circles back to the beginning into 2 entries
    U16 total_records = (TEST_TRACE_FILE_SIZE_MAX / FW_TRACE_MAX_SER_SIZE);
    U16 file_entries = total_records+2;
    U8 type_entry = 0; //to iterate over trace type 
    std::array<U8,11> buffer = {0x15,0x26,0x37,0x48,0x59,170,0xBB,0xCC,0xDD,0xEE,0xFF}; //arguments to write
    U8 *buff_ptr = &buffer[0];
    Fw::TraceBuffer trace_buffer_args(buff_ptr,buffer.size());
    
    for (int i = 0; i < file_entries; i++) {
        type_entry = (type_entry+1) % 4;
        timeTag.set(TB_DONT_CARE ,i,i+10);
        this->invoke_to_TraceBufferLogger(0,i,timeTag,static_cast<Fw::TraceCfg::TraceType::T>(type_entry),trace_buffer_args);
        this->component.doDispatch();
    }
    //Read file to the storage buffer
    this->read_file();
    
    //Iterate through the buffer and verify contents
    for(U16 i = 0 ; i < total_records ; i++ ) {
        //Verify Type ID
        U16 buff_ptr = i*FW_TRACE_MAX_SER_SIZE; 
        memcpy(&trace_id,&storage_buffer[buff_ptr+3],sizeof(trace_id));
        if(i == 0 || i == 1) { ASSERT_EQ(trace_id,total_records+i);}
        else ASSERT_EQ(trace_id,i);
        
        //Verify Time Tag
        memcpy(&time_store_type,&storage_buffer[buff_ptr+4],sizeof(time_store_type));
        memcpy(&time_secs,&storage_buffer[buff_ptr+10],sizeof(time_secs));
        memcpy(&time_usecs,&storage_buffer[buff_ptr+14],sizeof(time_usecs));
        ASSERT_EQ(time_store_type,TB_DONT_CARE);
        if(i == 0 || i == 1) { 
            ASSERT_EQ(time_secs,total_records+i);
            ASSERT_EQ(time_usecs,total_records+i+10);
        }
        else {
            ASSERT_EQ(time_secs,i);
            ASSERT_EQ(time_usecs,i+10);

        }

        //Verify Arguments
        memcpy(&arg_size,&storage_buffer[buff_ptr+16],sizeof(arg_size));
        memcpy(arg_check_ptr,&storage_buffer[buff_ptr+17],arg_check.size());
        ASSERT_EQ(arg_size,sizeof(buffer));
        ASSERT_TRUE(arg_check == buffer);
    }

}

void TraceFileLoggerTester :: test_filter_trace_type() {
    printf("Test Trace Type Filter\n");
    Fw::Time timeTag;
    U8 trace_id;
    U16 time_store_type;
    U8 time_secs;
    U8 time_usecs;
    U8 arg_size;
    std::array<U8,11> arg_check;
    U8* arg_check_ptr = &arg_check[0];
    std::array<U8,11> buffer = {0x15,0x26,0x37,0x48,0x59,170,0xBB,0xCC,0xDD,0xEE,0xFF}; //arguments to write
    U8 *buff_ptr = &buffer[0];
    Fw::TraceBuffer trace_buffer_args(buff_ptr,buffer.size());
    timeTag.set(TB_DONT_CARE ,0xBE,0xEF);
    
    
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

    this->read_file();
    U16 total_records = (this->file_size / FW_TRACE_MAX_SER_SIZE);
    printf ("TOTAL RECORDS : %d \n", total_records);

    //Iterate through buffer and verify contents
    for(U16 i = 0 ; i < total_records ; i++ ) {
        
        U16 buff_ptr = i*FW_TRACE_MAX_SER_SIZE; 
        memcpy(&trace_id,&storage_buffer[buff_ptr+3],sizeof(trace_id));
        //Verify Trace ID
        switch(i) {
            case 0:
                ASSERT_EQ(trace_id,0xAA);
                break;
            case 1:
                ASSERT_EQ(trace_id,0xDD);
                break;
            case 2:
                ASSERT_EQ(trace_id,0xAA);
                break;
            case 3:
                ASSERT_EQ(trace_id,0xBB);
                break;
            case 4:
                ASSERT_EQ(trace_id,0xCC);
                break;
            case 5:
                ASSERT_EQ(trace_id,0xDD);
                break;
        }

        //Verify time tag
        memcpy(&time_store_type,&storage_buffer[buff_ptr+4],sizeof(time_store_type));
        memcpy(&time_secs,&storage_buffer[buff_ptr+10],sizeof(time_secs));
        memcpy(&time_usecs,&storage_buffer[buff_ptr+14],sizeof(time_usecs));
        ASSERT_EQ(time_secs, 0xBE);
        ASSERT_EQ(time_usecs, 0xEF);
        ASSERT_EQ(time_store_type,0xFFFF);
        
        //Verify Arguments
        memcpy(&arg_size,&storage_buffer[buff_ptr+16],sizeof(arg_size));
        memcpy(arg_check_ptr,&storage_buffer[buff_ptr+17],arg_check.size());
        ASSERT_EQ(arg_size,sizeof(buffer)); //size of the arguments
        ASSERT_TRUE(arg_check == buffer); //Buffer data matches

    }
}

void TraceFileLoggerTester :: test_filter_trace_id() {
    printf("Test Trace ID Filtering Capability\n");
    Fw::Time timeTag;
    U8 trace_id;
    U16 time_store_type;
    U8 time_secs;
    U8 time_usecs;
    U8 arg_size;
    std::array<U8,11> arg_check;
    U8* arg_check_ptr = &arg_check[0];
    std::array<U8,11> buffer = {0x15,0x26,0x37,0x48,0x59,170,0xBB,0xCC,0xDD,0xEE,0xFF}; //arguments to write
    U8 *buff_ptr = &buffer[0];
    Fw::TraceBuffer trace_buffer_args(buff_ptr,buffer.size());
    timeTag.set(TB_DONT_CARE ,0xBE,0xEF);

    /*
    1. Send a few trace ids
    2. Verify they exist in log file
    3. Send command to disable a few trace ids; verify array
    4. Write to buffer logger and ensure they're filtered out
    5. Write duplicate commands and check trace id array that ids are unique
    6. Enable trace ids again, verify array
    7. Verify file that all of them exist. 
    */

    //Filter out trace ids 0xAA and 0xDD
    this->sendCmd_DisableTraceId(0,1,0xAA,Svc::TraceFileLogger_Enable::DISABLE);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE(0, 3, 1, Fw::CmdResponse::OK);
    this->sendCmd_DisableTraceId(0,1,0xDD,Svc::TraceFileLogger_Enable::DISABLE);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE(0, 3, 1, Fw::CmdResponse::OK);

    //Verify Array
    ASSERT_EQ(this->component.traceId_array[0], 0xAA); 
    ASSERT_EQ(this->component.traceId_array[1], 0xDD);
    
    //Send command with duplicate traceID and ensure the array only contains unique trace IDs
    this->sendCmd_DisableTraceId(0,1,0xDD,Svc::TraceFileLogger_Enable::DISABLE);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE(0, 3, 1, Fw::CmdResponse::OK);

    //Verify Array
    ASSERT_EQ(this->component.traceId_array[0], 0xAA); 
    ASSERT_EQ(this->component.traceId_array[1], 0xDD);
    ASSERT_EQ(this->component.traceId_array[3], 0x00);
    
    this->invoke_to_TraceBufferLogger(0,0xAA,timeTag,Fw::TraceCfg::TraceType::MESSAGE_QUEUE,trace_buffer_args);
    this->component.doDispatch();
    this->invoke_to_TraceBufferLogger(0,0xBB,timeTag,Fw::TraceCfg::TraceType::MESSAGE_DEQUEUE,trace_buffer_args);
    this->component.doDispatch();
    this->invoke_to_TraceBufferLogger(0,0xCC,timeTag,Fw::TraceCfg::TraceType::PORT_CALL,trace_buffer_args);
    this->component.doDispatch();
    this->invoke_to_TraceBufferLogger(0,0xDD,timeTag,Fw::TraceCfg::TraceType::USER,trace_buffer_args);
    this->component.doDispatch();

    //Filter out trace ids 0xAA and 0xDD
    this->sendCmd_DisableTraceId(0,1,0xAA,Svc::TraceFileLogger_Enable::ENABLE);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE(0, 3, 1, Fw::CmdResponse::OK);
    this->sendCmd_DisableTraceId(0,1,0xDD,Svc::TraceFileLogger_Enable::ENABLE);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE(0, 3, 1, Fw::CmdResponse::OK);
    this->sendCmd_DisableTraceId(0,1,0xCC,Svc::TraceFileLogger_Enable::DISABLE);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE(0, 3, 1, Fw::CmdResponse::OK);

    //enable all trace type filters and ensure they're received
    this->invoke_to_TraceBufferLogger(0,0xAA,timeTag,Fw::TraceCfg::TraceType::MESSAGE_QUEUE,trace_buffer_args);
    this->component.doDispatch();
    this->invoke_to_TraceBufferLogger(0,0xBB,timeTag,Fw::TraceCfg::TraceType::MESSAGE_DEQUEUE,trace_buffer_args);
    this->component.doDispatch();
    this->invoke_to_TraceBufferLogger(0,0xCC,timeTag,Fw::TraceCfg::TraceType::PORT_CALL,trace_buffer_args);
    this->component.doDispatch();
    this->invoke_to_TraceBufferLogger(0,0xDD,timeTag,Fw::TraceCfg::TraceType::USER,trace_buffer_args);
    this->component.doDispatch();

    this->read_file();
    U16 total_records = (this->file_size / FW_TRACE_MAX_SER_SIZE);
    printf ("TOTAL RECORDS : %d \n", total_records);

    //Iterate through buffer and verify contents
    for(U16 i = 0 ; i < total_records ; i++ ) {
        
        U16 buff_ptr = i*FW_TRACE_MAX_SER_SIZE; 
        memcpy(&trace_id,&storage_buffer[buff_ptr+3],sizeof(trace_id));
        //Verify Trace ID
        switch(i) {
            case 0:
                ASSERT_EQ(trace_id,0xBB);
                break;
            case 1:
                ASSERT_EQ(trace_id,0xCC);
                break;
            case 2:
                ASSERT_EQ(trace_id,0xAA);
                break;
            case 3:
                ASSERT_EQ(trace_id,0xBB);
                break;
            case 4:
                ASSERT_EQ(trace_id,0xDD);
                break;
            default:
                FW_ASSERT(false);
        }    

        //Verify time tag
        memcpy(&time_store_type,&storage_buffer[buff_ptr+4],sizeof(time_store_type));
        memcpy(&time_secs,&storage_buffer[buff_ptr+10],sizeof(time_secs));
        memcpy(&time_usecs,&storage_buffer[buff_ptr+14],sizeof(time_usecs));
        ASSERT_EQ(time_secs, 0xBE);
        ASSERT_EQ(time_usecs, 0xEF);
        ASSERT_EQ(time_store_type,0xFFFF);
        
        //Verify Arguments
        memcpy(&arg_size,&storage_buffer[buff_ptr+16],sizeof(arg_size));
        memcpy(arg_check_ptr,&storage_buffer[buff_ptr+17],arg_check.size());
        ASSERT_EQ(arg_size,sizeof(buffer)); //size of the arguments
        ASSERT_TRUE(arg_check == buffer); //Buffer data matches
    }
}

void TraceFileLoggerTester :: read_file() {
    std::ifstream trace_file("TraceFileTest.dat");//,std::ios::binary);
    
    trace_file.seekg(0, std::ios::end);
    std::streamsize size = trace_file.tellg();  // Get the size of the file
    printf("Size of the trace_file is %ld\n",size);
    this->file_size = size;
    trace_file.seekg(0, std::ios::beg);

    storage_buffer.resize(size);
    fill(storage_buffer.begin(),storage_buffer.end(),0);
    trace_file.read(reinterpret_cast<char *>(storage_buffer.data()),size);

    trace_file.close();

   /* 
    for(U8 byte : storage_buffer) {
        //std::cout << byte << " ";
        printf("0x%X ",(byte));

    } 
    */   
}

}  // namespace Svc
