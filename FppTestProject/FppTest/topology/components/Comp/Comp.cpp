// ======================================================================
// \title  Comp.cpp
// \author tumbar
// \brief  cpp file for Comp component implementation class
// ======================================================================

#include "FppTest/topology/components/Comp/Comp.hpp"

namespace FppTest {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

Comp ::Comp(const char* const compName)
    : CompComponentBase(compName), m_dpInProgress(false), m_opcode(0), m_cmdSeq(0) {}

Comp ::~Comp() = default;

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void Comp ::emitEvent(U32 a, F32 b, const Fw::StringBase& c) {
    log_ACTIVITY_HI_Event(a, b, c);
}

void Comp ::emitTelemetry(U32 a) {
    tlmWrite_Telemetry(a);
}

U32 Comp ::getParameter() {
    Fw::ParamValid valid;
    U32 out = paramGet_Param(valid);
    FW_ASSERT(valid == Fw::ParamValid::VALID);
    return out;
}

void Comp::parameterUpdated(FwPrmIdType id) {
    CompComponentBase::parameterUpdated(id);
    tlmWrite_ParamUpdated({id, getParameter()});
}

void Comp ::PingIn_handler(FwIndexType portNum, U32 key) {
    PingOut_out(portNum, key);
}

// ----------------------------------------------------------------------
// Handler implementations for commands
// ----------------------------------------------------------------------

void Comp ::Start_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, U32 nRecords) {
    if (m_dpInProgress) {
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::BUSY);
        return;
    }

    m_dpInProgress = true;
    m_opcode = opCode;
    m_cmdSeq = cmdSeq;
    dpRequest_Product(FixedSizeData::SERIALIZED_SIZE * nRecords);
}

void Comp ::Data_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, U32 a, F32 b, const Fw::CmdStringArg& c) {
    if (m_dpInProgress) {
        const auto status = m_dpContainer.serializeRecord_FixedSizeDataRecord(FixedSizeData(a, b, c));
        FW_ASSERT(status == Fw::SerializeStatus::FW_SERIALIZE_OK, status);
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    } else {
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::BUSY);
    }
}

void Comp ::End_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    if (m_dpInProgress) {
        dpSend(m_dpContainer);
        m_dpInProgress = false;
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    } else {
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::BUSY);
    }
}

void Comp::syncIn_handler(FwIndexType portNum, U32 context) {
    syncOut_out(portNum, context);
}

// ----------------------------------------------------------------------
// Handler implementations for data products
// ----------------------------------------------------------------------

void Comp ::dpRecv_Product_handler(DpContainer& container, Fw::Success::T status) {
    if (status == Fw::Success::SUCCESS) {
        m_dpContainer = container;
        this->cmdResponse_out(m_opcode, m_cmdSeq, Fw::CmdResponse::OK);
    } else {
        m_dpInProgress = false;
        this->cmdResponse_out(m_opcode, m_cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
    }
}

}  // namespace FppTest
