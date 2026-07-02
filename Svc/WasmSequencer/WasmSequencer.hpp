// ======================================================================
// \title  WasmSequencer.hpp
// \author tumbar
// \brief  hpp file for WasmSequencer component implementation class
// ======================================================================

#ifndef Svc_WasmSequencer_HPP
#define Svc_WasmSequencer_HPP

#include "Svc/WasmSequencer/WasmSequencerComponentAc.hpp"

namespace Svc {

class WasmSequencer final : public WasmSequencerComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct WasmSequencer object
    WasmSequencer(const char* const compName  //!< The component name
    );

    //! Destroy WasmSequencer object
    ~WasmSequencer();

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for cmdResponseIn
    //!
    //! Response
    void cmdResponseIn_handler(FwIndexType portNum,             //!< The port number
                               FwOpcodeType opCode,             //!< Command Op Code
                               U32 cmdSeq,                      //!< Command Sequence
                               const Fw::CmdResponse& response  //!< The command response argument
                               ) override;

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for commands
    // ----------------------------------------------------------------------

    //! Handler implementation for command RUN
    //!
    //! Run a Wasm module main function on it's own in the interpreter
    //! This command is short-hand for:
    //! 1. CLEAR_STORE
    //! 2. LOAD_NAME [fileName] ""
    //! 3. INVOKE "" "main"
    //! 4. CONTINUE
    //!
    //! If $block == Svc.BlockState.BLOCK this command will wait for complemention.
    void RUN_cmdHandler(FwOpcodeType opCode,               //!< The opcode
                        U32 cmdSeq,                        //!< The command sequence number
                        const Fw::CmdStringArg& fileName,  //!< The name of the sequence file
                        Svc::BlockState block              //!< Return command status when complete or not
                        ) override;

    //! Handler implementation for command WAIT
    //!
    //! Wait for the interpreter to finish and return it's result as a CmdResponse
    void WAIT_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                         U32 cmdSeq            //!< The command sequence number
                         ) override;

    //! Handler implementation for command LOAD
    //!
    //! Loads and validates a WebAssembly module into the store.
    //! This command loads the module with a empty name meaning only a single module may be loaded.
    //! To allow multiple modules, use the `LOAD_NAME` command instead.
    void LOAD_cmdHandler(FwOpcodeType opCode,              //!< The opcode
                         U32 cmdSeq,                       //!< The command sequence number
                         const Fw::CmdStringArg& fileName  //!< The name of the sequence file
                         ) override;

    //! Handler implementation for command LOAD_NAME
    //!
    //! Load and validate a WebAssembly module into the store. This module is given a name so that
    //! it's exports can be referenced by other modules.
    void LOAD_NAME_cmdHandler(
        FwOpcodeType opCode,               //!< The opcode
        U32 cmdSeq,                        //!< The command sequence number
        const Fw::CmdStringArg& fileName,  //!< The name of the sequence file
        const Fw::CmdStringArg& name  //!< WebAssembly module name, must not conflict with previously loaded modules
        ) override;

    //! Handler implementation for command INVOKE
    //!
    //! Invoke a function from a loaded module
    void INVOKE_cmdHandler(FwOpcodeType opCode,                  //!< The opcode
                           U32 cmdSeq,                           //!< The command sequence number
                           const Fw::CmdStringArg& module,       //!< Name of the module to invoke a function from
                           const Fw::CmdStringArg& functionName  //!< Name of the function to be invoked
                           ) override;

    //! Handler implementation for command CLEAR_STORE
    //!
    //! Clear the WebAssembly store of all modules, tables, and memories to start fresh.
    void CLEAR_STORE_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                                U32 cmdSeq            //!< The command sequence number
                                ) override;

    //! Handler implementation for command CANCEL
    //!
    //! Cancels a running or validated sequence. After running CANCEL, the sequencer
    //! should return to IDLE
    void CANCEL_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                           U32 cmdSeq            //!< The command sequence number
                           ) override;

    //! Handler implementation for command BREAK
    //!
    //! Pauses the execution of the sequencer, just before it is about to dispatch the next directive,
    //! until unpaused by the CONTINUE command, or stepped by the STEP command. This command is only valid
    //! substates of the RUNNING state that are not RUNNING.PAUSED.
    void BREAK_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                          U32 cmdSeq            //!< The command sequence number
                          ) override;

    //! Handler implementation for command CONTINUE
    //!
    //! Continues the automatic execution of the sequence after it has been paused. If a breakpoint is still
    //! set, it may pause again on that breakpoint. This command is only valid in the RUNNING.PAUSED state.
    void CONTINUE_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                             U32 cmdSeq            //!< The command sequence number
                             ) override;
};

}  // namespace Svc

#endif
