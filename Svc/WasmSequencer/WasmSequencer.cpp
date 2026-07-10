// ======================================================================
// \title  WasmSequencer.cpp
// \author tumbar
// \brief  cpp file for WasmSequencer component implementation class
// ======================================================================

#include "Svc/WasmSequencer/WasmSequencer.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

WasmSequencer ::WasmSequencer(const char* const compName) : WasmSequencerComponentBase(compName) {}

WasmSequencer ::~WasmSequencer() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void WasmSequencer ::checkTimers_handler(FwIndexType portNum, U32 context) {
    // TODO
}

void WasmSequencer ::cmdResponseIn_handler(FwIndexType portNum,
                                           FwOpcodeType opCode,
                                           U32 cmdSeq,
                                           const Fw::CmdResponse& response) {
    // TODO
}

// ----------------------------------------------------------------------
// Handler implementations for commands
// ----------------------------------------------------------------------

void WasmSequencer ::RUN_cmdHandler(FwOpcodeType opCode,
                                    U32 cmdSeq,
                                    const Fw::CmdStringArg& fileName,
                                    Svc::BlockState block) {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void WasmSequencer ::WAIT_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void WasmSequencer ::LOAD_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, const Fw::CmdStringArg& fileName) {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void WasmSequencer ::LOAD_NAME_cmdHandler(FwOpcodeType opCode,
                                          U32 cmdSeq,
                                          const Fw::CmdStringArg& fileName,
                                          const Fw::CmdStringArg& name) {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void WasmSequencer ::INVOKE_cmdHandler(FwOpcodeType opCode,
                                       U32 cmdSeq,
                                       const Fw::CmdStringArg& module,
                                       const Fw::CmdStringArg& functionName) {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void WasmSequencer ::CLEAR_STORE_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void WasmSequencer ::REINITIALIZE_STORE_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, U16 module_count) {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void WasmSequencer ::CANCEL_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void WasmSequencer ::BREAK_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void WasmSequencer ::TRACE_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void WasmSequencer ::CONTINUE_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_signalEntered(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_invoke(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_setLoad(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal,
    const Svc::WasmSequencer_ModuleLoad& value) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_load(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_clearStore(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_reportValidationFailure(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_sendCmdResponse_VALIDATION_ERROR(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_pushModuleToStore(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_spin(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_report_seqSucceeded(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_report_seqCancelled(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_report_seqFailed(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_report_seqTrap(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_report_seqReadError(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_report_seqBroken(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_sendCmdResponse_OK(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_sendCmdResponse_EXECUTION_ERROR(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_set_sleepTime(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_checkStatementTimeout(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_checkShouldWake(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_setBreakBeforeNextLine(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_clearBreakBeforeNextLine(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

// ----------------------------------------------------------------------
// Implementations for internal state machine guards
// ----------------------------------------------------------------------

bool WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_guard_pendingInvoke(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) const {
    // TODO
}

bool WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_guard_hasStartFunction(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) const {
    // TODO
}

bool WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_guard_shouldBreak(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) const {
    // TODO
}

}  // namespace Svc
