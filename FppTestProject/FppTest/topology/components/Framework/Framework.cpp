// ======================================================================
// \title  Framework.cpp
// \author tumbar
// \brief  cpp file for Framework component implementation class
// ======================================================================

#include "FppTest/topology/components/Framework/Framework.hpp"

namespace FppTest {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

Framework ::Framework(const char* const compName) : FrameworkComponentBase(compName) {
    for (FwSizeType i = 0; i < 5; i++) {
        m_memory_queue.enqueue(Fw::Buffer(m_memory_pool[i], 1024));
    }
}

Framework ::~Framework() = default;

void Framework ::init(FwSizeType queueDepth, FwEnumStoreType instance) {
    FrameworkComponentBase::init(queueDepth, instance);
    m_completion_queue.create(0, Fw::String("Framework completion queue"), 1, 4);
}

void Framework::setTime(const Fw::Time& time) {
    m_time = time;
}

Fw::CmdResponse Framework::sendCommand(FwOpcodeType opcode, U32 cmdSeq, Fw::CmdArgBuffer& args) {
    FwIndexType portNum;
    auto portNumStatus = m_opcode_registrations.find(opcode, portNum);
    FW_ASSERT(portNumStatus == Fw::Success::SUCCESS, portNumStatus);

    compCmdSend_out(portNum, opcode, cmdSeq, args);

    U8 bufData[4];
    Fw::ExternalSerializeBuffer buf(bufData, sizeof(bufData));
    FwQueuePriorityType prio;

    // Block and wait for the queue to get a message
    auto status = m_completion_queue.receive(buf, Os::QueueInterface::BLOCKING, prio);
    FW_ASSERT(status == Os::QueueInterface::OP_OK, status);

    Fw::CmdResponse out;
    buf.deserializeTo(out);
    return out;
}

void Framework::ping(const U32 key) {
    for (FwIndexType i = 0; i < NUM_PINGSEND_OUTPUT_PORTS; i++) {
        PingSend_out(i, key);
        syncFor(i);
    }
}

void Framework::sync() {
    syncFor(0);
    syncFor(1);
}

void Framework::syncFor(FwIndexType portNum) {
    syncOut_out(portNum, 0);

    U8 bufData[4];
    Fw::ExternalSerializeBuffer buf(bufData, sizeof(bufData));
    FwQueuePriorityType prio;

    // Block and wait for the queue to get a message
    auto status = m_completion_queue.receive(buf, Os::QueueInterface::BLOCKING, prio);
    FW_ASSERT(status == Os::QueueInterface::OP_OK, status);

    FwIndexType gotPortNum;
    auto deStatus = buf.deserializeTo(gotPortNum);
    FW_ASSERT(deStatus == Fw::FW_SERIALIZE_OK, deStatus);

    FW_ASSERT(gotPortNum == portNum, gotPortNum, portNum);
}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void Framework::syncIn_handler(FwIndexType portNum, U32 context) {
    U8 bufData[4];
    Fw::ExternalSerializeBuffer buf(bufData, sizeof(bufData));

    buf.serializeFrom(portNum);

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
    log.set_portNum(portNum);
    log.set_id(id);
    log.set_timeTag(timeTag);
    log.set_severity(severity);
    log.set_args(FrameworkPortData::DataBuffer(args.getBuffAddr(), args.getSize()));
    log_queue.enqueue(log);
}

Fw::ParamValid Framework ::ParamGetIn_handler(FwIndexType portNum, FwPrmIdType id, Fw::ParamBuffer& val) {
    FrameworkPortData::PrmGet prmGet;
    prmGet.set_portNum(portNum);
    prmGet.set_id(id);
    prmGet.set_val(FrameworkPortData::DataBuffer(val.getBuffAddr(), val.getSize()));
    prm_get_queue.enqueue(prmGet);
    return Fw::ParamValid::VALID;
}

void Framework ::ParamSetIn_handler(FwIndexType portNum, FwPrmIdType id, Fw::ParamBuffer& val) {
    FrameworkPortData::PrmSet prmSet;
    prmSet.set_portNum(portNum);
    prmSet.set_id(id);
    prmSet.set_val(FrameworkPortData::DataBuffer(val.getBuffAddr(), val.getSize()));
    prm_set_queue.enqueue(prmSet);
}

void Framework ::PingReturn_handler(FwIndexType portNum, U32 key) {
    FrameworkPortData::Ping ping;
    ping.set_portNum(portNum);
    ping.set_key(key);
    ping_queue.enqueue(ping);
}

void Framework ::TextLogRecv_handler(FwIndexType portNum,
                                     FwEventIdType id,
                                     Fw::Time& timeTag,
                                     const Fw::LogSeverity& severity,
                                     Fw::TextLogString& text) {
    FrameworkPortData::LogText logText;
    logText.set_portNum(portNum);
    logText.set_id(id);
    logText.set_timeTag(timeTag);
    logText.set_severity(severity);
    logText.set_text(text);
    log_text_queue.enqueue(logText);
}

void Framework ::TlmRecv_handler(FwIndexType portNum, FwChanIdType id, Fw::Time& timeTag, Fw::TlmBuffer& val) {
    FrameworkPortData::Tlm tlm;
    tlm.set_portNum(portNum);
    tlm.set_id(id);
    tlm.set_timeTag(timeTag);
    tlm.set_val(FrameworkPortData::DataBuffer(val.getBuffAddr(), val.getSize()));
    tlm_queue.enqueue(tlm);
}

void Framework ::compCmdReg_handler(FwIndexType portNum, FwOpcodeType opCode) {
    FrameworkPortData::CmdReg cmdReg;
    cmdReg.set_portNum(portNum);
    cmdReg.set_opCode(opCode);
    cmd_reg_queue.enqueue(cmdReg);

    auto status = m_opcode_registrations.insert(opCode, portNum);
    FW_ASSERT(status == Fw::Success::SUCCESS);
}

void Framework ::compCmdStat_handler(FwIndexType portNum,
                                     FwOpcodeType opCode,
                                     U32 cmdSeq,
                                     const Fw::CmdResponse& response) {
    FrameworkPortData::CmdResponse cmdResponse;
    cmdResponse.set_portNum(portNum);
    cmdResponse.set_opCode(opCode);
    cmdResponse.set_cmdSeq(cmdSeq);
    cmdResponse.set_response(response);
    cmd_response_queue.enqueue(cmdResponse);

    U8 bufData[4];
    Fw::ExternalSerializeBuffer buf(bufData, sizeof(bufData));

    auto serStatus = buf.serializeFrom(response);
    FW_ASSERT(serStatus == Fw::FW_SERIALIZE_OK, serStatus);

    // Only a single completion signal should be sent out
    // This should not block on the queue which has a '1' depth
    auto status = m_completion_queue.send(buf, 0, Os::QueueInterface::NONBLOCKING);
    FW_ASSERT(status == Os::QueueInterface::OP_OK);
}

Fw::Success Framework ::productGetIn_handler(FwIndexType portNum,
                                             FwDpIdType id,
                                             FwSizeType dataSize,
                                             Fw::Buffer& buffer) {
    FrameworkPortData::DpGet dpGet;
    dpGet.set_portNum(portNum);
    dpGet.set_id(id);
    dpGet.set_dataSize(dataSize);
    dpGet.set_buffer({buffer.getData(), buffer.getSize()});
    dp_get_queue.enqueue(dpGet);
    return Fw::Success::SUCCESS;
}

void Framework ::productRequestIn_handler(FwIndexType portNum, FwDpIdType id, FwSizeType dataSize) {
    FrameworkPortData::DpRequest dpRequest;
    dpRequest.set_portNum(portNum);
    dpRequest.set_id(id);
    dpRequest.set_dataSize(dataSize);
    dp_request_queue.enqueue(dpRequest);

    Fw::Buffer next;
    if (dataSize > 1024) {
        productResponseOut_out(portNum, id, next, Fw::Success::FAILURE);
    } else {
        productResponseOut_out(portNum, id, next, m_memory_queue.dequeue(next));
    }
}

void Framework ::timeGetIn_handler(FwIndexType portNum, Fw::Time& time) {
    time = m_time;
}

void Framework::productSendIn_handler(FwIndexType portNum, FwDpIdType id, const Fw::Buffer& buffer) {
    FrameworkPortData::DpSend dpSend;
    dpSend.set_portNum(portNum);
    dpSend.set_id(id);
    dpSend.set_buffer({buffer.getData(), buffer.getSize()});
    dp_send_queue.enqueue(dpSend);
}

}  // namespace FppTest
