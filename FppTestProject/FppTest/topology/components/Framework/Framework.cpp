// ======================================================================
// \title  Framework.cpp
// \author tumbar
// \brief  cpp file for Framework component implementation class
// ======================================================================

#include "FppTest/topology/components/Framework/Framework.hpp"
#include "Framework.hpp"

namespace FppTest {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

Framework ::Framework(const char* const compName) : FrameworkComponentBase(compName) {}

Framework ::~Framework() {}

void Framework ::init(FwSizeType queueDepth, FwEnumStoreType instance) {
    FrameworkComponentBase::init(queueDepth, instance);
    m_completion_queue.create(0, Fw::String("Framework completion queue"), 1, 1);
}

void Framework::wait() {
    U8 bufData[1];
    Fw::ExternalSerializeBuffer buf(bufData, 1);
    FwQueuePriorityType prio;

    // Block and wait for the queue to get a message
    auto status = m_completion_queue.receive(buf, Os::QueueInterface::BLOCKING, prio);
    FW_ASSERT(status == Os::QueueInterface::OP_OK);
}

void Framework::clear() {
    cmd_reg_queue.clear();
    cmd_response.clear();
    log_queue.clear();
    log_text_queue.clear();
    tlm_queue.clear();
    prm_get_queue.clear();
    prm_set_queue.clear();
    time_queue.clear();
    dp_get_queue.clear();
    dp_request_queue.clear();
    ping_queue.clear();
}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void Framework ::Finish_handler(FwIndexType portNum, U32 context) {
    FW_ASSERT(portNum == 0, portNum);

    U8 bufData[1];
    Fw::ExternalSerializeBuffer buf(bufData, 1);

    // Only a single completion signal should be sent out
    // This should not block on the queue which has a '1' depth
    auto status = m_completion_queue.send(buf, 0, Os::QueueInterface::NONBLOCKING);
    FW_ASSERT(status == Os::QueueInterface::OP_OK);
}

void Framework ::LogRecv_handler(FwIndexType portNum,
                                 FwEventIdType id,
                                 Fw::Time& timeTag,
                                 const Fw::LogSeverity& severity,
                                 Fw::LogBuffer& args) {
    FrameworkPortData::Log log;
    log.set_id(id);
    log.set_timeTag(timeTag);
    log.set_severity(severity);
    log.set_args(Fw::Buffer(args.getBuffAddr(), args.getSize()));
    log_queue.enqueue(log);
}

Fw::ParamValid Framework ::ParamGetIn_handler(FwIndexType portNum, FwPrmIdType id, Fw::ParamBuffer& val) {
    FrameworkPortData::PrmGet prmGet;
    prmGet.set_id(id);
    prmGet.set_val(Fw::Buffer(val.getBuffAddr(), val.getSize()));
    prm_get_queue.enqueue(prmGet);
    return Fw::ParamValid::VALID;
}

void Framework ::ParamSetIn_handler(FwIndexType portNum, FwPrmIdType id, Fw::ParamBuffer& val) {
    FrameworkPortData::PrmSet prmSet;
    prmSet.set_id(id);
    prmSet.set_val(Fw::Buffer(val.getBuffAddr(), val.getSize()));
    prm_set_queue.enqueue(prmSet);
}

void Framework ::PingReturn_handler(FwIndexType portNum, U32 key) {
    FrameworkPortData::Ping ping;
    ping.set_key(key);
    ping_queue.enqueue(ping);
}

void Framework ::TextLogRecv_handler(FwIndexType portNum,
                                     FwEventIdType id,
                                     Fw::Time& timeTag,
                                     const Fw::LogSeverity& severity,
                                     Fw::TextLogString& text) {
    FrameworkPortData::LogText logText;
    logText.set_id(id);
    logText.set_timeTag(timeTag);
    logText.set_severity(severity);
    logText.set_text(text);
    log_text_queue.enqueue(logText);
}

void Framework ::TlmRecv_handler(FwIndexType portNum, FwChanIdType id, Fw::Time& timeTag, Fw::TlmBuffer& val) {
    FrameworkPortData::Tlm tlm;
    tlm.set_id(id);
    tlm.set_timeTag(timeTag);
    tlm.set_val(Fw::Buffer(val.getBuffAddr(), val.getSize()));
    tlm_queue.enqueue(tlm);
}

void Framework ::compCmdReg_handler(FwIndexType portNum, FwOpcodeType opCode) {
    FrameworkPortData::CmdReg cmdReg;
    cmdReg.set_opCode(opCode);
    cmd_reg_queue.enqueue(cmdReg);
}

void Framework ::compCmdStat_handler(FwIndexType portNum,
                                     FwOpcodeType opCode,
                                     U32 cmdSeq,
                                     const Fw::CmdResponse& response) {
    FrameworkPortData::CmdResponse cmdResponse;
    cmdResponse.set_opCode(opCode);
    cmdResponse.set_cmdSeq(cmdSeq);
    cmdResponse.set_response(response);
    cmd_response.enqueue(cmdResponse);
}

Fw::Success Framework ::productGetIn_handler(FwIndexType portNum,
                                             FwDpIdType id,
                                             FwSizeType dataSize,
                                             Fw::Buffer& buffer) {
    FrameworkPortData::DpGet dpGet;
    dpGet.set_id(id);
    dpGet.set_dataSize(dataSize);
    dpGet.set_buffer(buffer);
    dp_get_queue.enqueue(dpGet);
    return Fw::Success::SUCCESS;
}

void Framework ::productRequestIn_handler(FwIndexType portNum, FwDpIdType id, FwSizeType dataSize) {
    FrameworkPortData::DpRequest dpRequest;
    dpRequest.set_id(id);
    dpRequest.set_dataSize(dataSize);
    dp_request_queue.enqueue(dpRequest);
}

void Framework ::timeGetIn_handler(FwIndexType portNum, Fw::Time& time) {
    time.set(0);

    FrameworkPortData::Time timeData;
    timeData.set_time(time);
    time_queue.enqueue(timeData);
}

}  // namespace FppTest
