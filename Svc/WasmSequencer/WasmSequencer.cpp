// ======================================================================
// \title  WasmSequencer.cpp
// \author tumbar
// \brief  cpp file for WasmSequencer component implementation class
// ======================================================================

#include "Svc/WasmSequencer/WasmSequencer.hpp"

#include "Svc/WasmSequencer/fprime_spacewasm/fprime_spacewasm.h"

namespace Svc {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

WasmSequencer ::WasmSequencer(const char* const compName) : WasmSequencerComponentBase(compName) {
    // Scaffolding: exercise the fprime_spacewasm FFI so the Rust static library
    // link path is verified by a normal build. Replace with real interpreter
    // setup as functionality is added.
    (void)fprime_spacewasm_probe();
}

WasmSequencer ::~WasmSequencer() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void WasmSequencer ::cmdResponseIn_handler(FwIndexType portNum,
                                           FwOpcodeType opCode,
                                           U32 cmdSeq,
                                           const Fw::CmdResponse& response) {
    // TODO
}

}  // namespace Svc
