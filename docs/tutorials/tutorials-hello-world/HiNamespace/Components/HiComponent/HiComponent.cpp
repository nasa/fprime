// ======================================================================
// \title  HiComponent.cpp
// \author djbyrne
// \brief  cpp file for HiComponent component implementation class
// ======================================================================

#include "HiNamespace/Components/HiComponent/HiComponent.hpp"

namespace HiNamespace {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

HiComponent ::HiComponent(const char* const compName) : HiComponentComponentBase(compName) {}

HiComponent ::~HiComponent() {}

// ----------------------------------------------------------------------
// Handler implementations for commands
// ----------------------------------------------------------------------

void HiComponent ::SAY_HI_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, const Fw::CmdStringArg& greeting) {
    // Copy the command string input into an event string for the SayHiEvent event
    Fw::LogStringArg eventGreeting(greeting.toChar());
    // Emit the SayHiEvent event with the copied string
    this->log_ACTIVITY_HI_SayHiEvent(eventGreeting);

    this->tlmWrite_GreetingCount(++this->m_greetingCount);

    // Tell the fprime command system that we have completed the processing of the supplied command with OK status
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

}  // namespace HiNamespace
