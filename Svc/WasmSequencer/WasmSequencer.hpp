// ======================================================================
// \title  WasmSequencer.hpp
// \author tumbar
// \brief  hpp file for WasmSequencer component implementation class
// ======================================================================

#ifndef Svc_WasmSequencer_HPP
#define Svc_WasmSequencer_HPP

#include "Fw/Cmd/CmdResponseEnumAc.hpp"
#include "Fw/DataStructures/FifoQueue.hpp"
#include "Fw/Types/FileNameString.hpp"
#include "Fw/Types/MemAllocator.hpp"
#include "Fw/Types/Serializable.hpp"
#include "Fw/Types/StringBase.hpp"
#include "Fw/Types/SuccessEnumAc.hpp"
#include "Svc/Seq/SeqArgsSerializableAc.hpp"
#include "Svc/WasmSequencer/WasmSequencerComponentAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_HostFunctionEnumAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_ModuleIdxAliasAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_RequestContextSerializableAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_TrapReasonEnumAc.hpp"
#include "Svc/WasmSequencer/spacewasm_include/spacewasm.h"
#include "Utils/Types/CircularBuffer.hpp"
#include "config/FwChanIdTypeAliasAc.h"
#include "config/FwPrmIdTypeAliasAc.h"
#include "config/FwSizeTypeAliasAc.h"
#include "config/WasmSequencerConfig.hpp"

namespace Svc {

class WasmSequencer final : public WasmSequencerComponentBase {
    friend class WasmSequencerTester;

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct WasmSequencer object
    WasmSequencer(const char* const compName  //!< The component name
    );

    //! Destroy WasmSequencer object
    ~WasmSequencer() = default;

    //! WasmSequencer owns a raw spacewasm_t* handle and a slot in the
    //! process-wide global-allocator registry; copying would double-free both.
    WasmSequencer(const WasmSequencer&) = delete;
    WasmSequencer& operator=(const WasmSequencer&) = delete;

    //! Configuration to select what happens when a serialIn fills
    enum class SerialInQueueFullBehavior {
        DROP_NEWEST,  //!< Drop the latest message if it cannot fit in the remaining queue space
        DROP_OLDEST,  //!< Oldest message[s] will be de-queued and dropped to make space for the new message. Keep
                      //!< dropping messages until enough space is made
        ASSERT,       //!< Trigger an assertion if the queue fills and cannot process another message
    };

    //! SpaceWasm has a hard-coded memory alignment requirement
    constexpr static FwSizeType SPACEWASM_MEMORY_ALIGNMENT = 8;

    //! Heap pages must be able to hold IR pages (512 bytes) minimum
    //! type IrWord = U16
    //! type IrPage = [256] IrWord => sizeof(IrPage) == 512
    constexpr static FwSizeType SPACEWASM_IR_PAGE_SIZE = 512;
    static_assert(Svc::WasmSequencerConfig::SPACEWASM_PAGE_SIZE >= SPACEWASM_IR_PAGE_SIZE,
                  "SpaceWasm does not support dynamic memory pages smaller than a single IR page (512 bytes)");

    //! Per-port serialIn queue configuration.
    struct SerialInQueueConfig {
        //! Queue size in bytes. A port left at size 0 gets no queue (all inbound frames on that index are dropped).
        FwSizeType size = 0;
        //! Overflow policy applied when a new frame does not fit.
        SerialInQueueFullBehavior fullBehavior = SerialInQueueFullBehavior::DROP_NEWEST;
    };

    //! Memory resource configuration for WasmSequencer.
    //! See the [sizing guide](docs/sdd.md#sizing-guide) for how to configure this.
    struct Config {
        //! Number of `WASM_SEQ_SPACEWASM_PAGE_SIZE` pages to allocate
        //! for the backing heap memory pool.
        //! This is used for store the loaded Wasm modules and their IR (executable code)
        //! but NOT for the guest memory (linear memory).
        FwSizeType heapPages = 8;

        //! Guest linear memory pool shared across all loaded modules
        //! Note that a `.wasm` module will specify its linear memory size up-front in multiples of page-sizes
        //! By default Wasm pages are 64k. if custom-page-sizes is enabled, page sizes are 1 byte.
        FwSizeType guestMemorySize = 8192;

        //! Wasm operand stack. The operand stack holds function parameters, locals and temporary operands.
        //! When Wasm code calls another function, its frame will be pushed to the stack.
        //!
        //! This size is in units of 32-bit words.
        //! The Wasm stack will be allocated from the heap memory pool
        FwSizeType stackSize = 1024;

        //! Allocates an empty list of pointers into the heap (i.e. sizeof(void*) * maxCodePages capacity).
        //! A fixed capacity for holding decoded Wasm executable instructions in units of _Code Pages_.
        //! Each page is 512 bytes (256 16-bit words) where each word holds a single resolved instruction (intermediate
        //! representation - IR).
        //!
        //! This should be sized large enough to hold all the code pages you will need.
        //! Note that this does not allocate the code pages themselves, it just incurs a `maxCodePages * sizeof(void*)`
        //! allocates to the heap. From there, each code page (512 bytes) will be lazily allocated to the heap as needed
        //! by .wasm modules.
        //!
        //! The larger this number is the higher overhead needed in the Wasm heap.
        U32 maxCodePages = 256;

        //! Maximum number of Wasm modules that may be loaded into the sequencer's store.
        U8 maxGuestModules = 8;

        //! Size (in bytes) of the serialOut buffer used to copy a `serial_send` payload out of guest memory
        //! before invoking the connected serialOut port. A value of 0 leaves serialOut unconfigured (disabled
        //! serial_send).
        FwSizeType serialOutMax = 0;

        //! Per-port serialIn queue sizing and overflow policy. Default-construct the Config then populate
        //! per port index, e.g.
        //! `Config cfg; cfg.serialIn[0] = {256, SerialInQueueFullBehavior::DROP_OLDEST};`
        //! A port left at size 0 gets no queue (all inbound frames on that index are dropped).
        SerialInQueueConfig serialIn[NUM_SERIALIN_INPUT_PORTS];
    };

    //! [REQUIRED] Configure and allocate the dynamic backing pools for heap memory, guest memory, Wasm stack,
    //! serialIn queues, and the serialOut buffer.
    void configure(const Config& cfg, Fw::MemAllocator& mallocator);

    //! Tear down the allocations made by `configure()`
    void deinit() override;

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for checkTimers
    //!
    //! Port to periodically drive sleep-wake and host-function-timeout checks
    void checkTimers_handler(FwIndexType portNum,  //!< The port number
                             U32 context           //!< The call order
                             ) override;

    //! Handler implementation for cmdResponseIn
    //!
    //! Command response input
    void cmdResponseIn_handler(FwIndexType portNum,             //!< The port number
                               FwOpcodeType opCode,             //!< Command Op Code
                               U32 cmdSeq,                      //!< Command Sequence
                               const Fw::CmdResponse& response  //!< The command response argument
                               ) override;

    //! Handler implementation for seqCancelIn
    //!
    //! port for requesting to cancel the currently running sequence
    void seqCancelIn_handler(FwIndexType portNum  //!< The port number
                             ) override;

    //! Handler implementation for seqRunIn
    //!
    //! port for requests to run sequences
    void seqRunIn_handler(FwIndexType portNum,             //!< The port number
                          const Fw::StringBase& filename,  //!< The sequence file
                          const Svc::SeqArgs& args         //!< Sequence arguments
                          ) override;

    //! Handler implementation for writeTelemetry
    //!
    //! Port to periodically write telemetry channels (optional)
    void writeTelemetry_handler(FwIndexType portNum,  //!< The port number
                                U32 context           //!< The call order
                                ) override;

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for serial input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for serialIn
    //!
    //! Port for receiving serial messages from other components.
    //! All port indices will be placed into their own internal binary queue to be
    //! handled by the serial_recv host function.
    void serialIn_handler(FwIndexType portNum,          //!< The port number
                          Fw::LinearBufferBase& buffer  //!< The serialization buffer
                          ) override;

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for commands
    // ----------------------------------------------------------------------

    //! Handler implementation for command RUN
    //!
    //! Run a Wasm module main function on it's own in the interpreter
    //! This command first resets the store (discarding any modules previously staged
    //! with LOAD), then is short-hand for:
    //!
    //! ```sh
    //! CANCEL # doesn't actually cancel a sequence, just resets the store
    //! LOAD [fileName] ""
    //! INVOKE "" [block] [seqArgs]
    //! ```
    //!
    //! If $block == Svc.BlockState.BLOCK this command will wait for completion.
    void RUN_cmdHandler(
        FwOpcodeType opCode,               //!< The opcode
        U32 cmdSeq,                        //!< The command sequence number
        const Fw::CmdStringArg& fileName,  //!< The name of the sequence file
        const Svc::BlockState& block,      //!< Block until sequence has finished running
        const Svc::SeqArgs& seqArgs        //!< Optional arguments to execute the sequence with
                                           //!< Depending on the sequence being loaded these arguments may differ
        ) override;

    //! Handler implementation for command LOAD
    //!
    //! Loads and validates a WebAssembly module into the store under the given name.
    //! Naming the module lets its exports be referenced by other modules and lets
    //! INVOKE, GLOBAL_GET and GLOBAL_SET address it. Use an empty name for a single,
    //! standalone module. The name must not conflict with a previously loaded module.
    void LOAD_cmdHandler(FwOpcodeType opCode,               //!< The opcode
                         U32 cmdSeq,                        //!< The command sequence number
                         const Fw::CmdStringArg& fileName,  //!< The name of the sequence file
                         const Fw::CmdStringArg& name  //!< WebAssembly module name (empty for a single unnamed module)
                         ) override;

    //! Handler implementation for command INVOKE
    //!
    //! Invoke a main function from a loaded module
    void INVOKE_cmdHandler(FwOpcodeType opCode,             //!< The opcode
                           U32 cmdSeq,                      //!< The command sequence number
                           const Fw::CmdStringArg& module,  //!< Name of the module to invoke a function from
                           const Svc::BlockState& block,    //!< Block until sequence has finished running
                           const Svc::SeqArgs& seqArgs      //!< Arguments to invoke the sequence entrypoint with
                           ) override;

    //! Handler implementation for command WAIT
    //!
    //! Wait for the interpreter to finish and return it's result as a CmdResponse
    void WAIT_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                         U32 cmdSeq            //!< The command sequence number
                         ) override;

    //! Handler implementation for command CANCEL
    //!
    //! Cancels a running or validated sequence. After running CANCEL, the sequencer should return to IDLE
    //! This completely clears the store.
    void CANCEL_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                           U32 cmdSeq            //!< The command sequence number
                           ) override;

    //! Handler implementation for command PAUSE
    //!
    //! Pauses the execution of the sequencer, just before it is about to start spinning.
    //! This simply pends a pause flag that will be taken before the sequence engine starts
    //! up again.
    //!
    //! This command completes immediately
    void PAUSE_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                          U32 cmdSeq            //!< The command sequence number
                          ) override;

    //! Handler implementation for command CONTINUE
    //!
    //! Resume the sequence from a paused state
    void CONTINUE_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                             U32 cmdSeq            //!< The command sequence number
                             ) override;

    //! Handler implementation for command GLOBAL_SET_I32
    //!
    //! Set a global variable to a given i32 value.
    //! Command fails if the module is not found, global is not exported, mutable, or of i32 type
    void GLOBAL_SET_I32_cmdHandler(FwOpcodeType opCode,                 //!< The opcode
                                   U32 cmdSeq,                          //!< The command sequence number
                                   const Fw::CmdStringArg& moduleName,  //!< Name of the module to set global for
                                   const Fw::CmdStringArg& name,        //!< Name of the global
                                   I32 value                            //!< Value to set global to
                                   ) override;

    //! Handler implementation for command GLOBAL_SET_I64
    //!
    //! Set a global variable to a given i64 value.
    //! Command fails if the module is not found, global is not exported, mutable, or of i64 type
    void GLOBAL_SET_I64_cmdHandler(FwOpcodeType opCode,                 //!< The opcode
                                   U32 cmdSeq,                          //!< The command sequence number
                                   const Fw::CmdStringArg& moduleName,  //!< Name of the module to set global for
                                   const Fw::CmdStringArg& name,        //!< Name of the global
                                   I64 value                            //!< Value to set global to
                                   ) override;

    //! Handler implementation for command GLOBAL_SET_F32
    //!
    //! Set a global variable to a given f32 value.
    //! Command fails if the module is not found, global is not exported, mutable, or of f32 type
    void GLOBAL_SET_F32_cmdHandler(FwOpcodeType opCode,                 //!< The opcode
                                   U32 cmdSeq,                          //!< The command sequence number
                                   const Fw::CmdStringArg& moduleName,  //!< Name of the module to set global for
                                   const Fw::CmdStringArg& name,        //!< Name of the global
                                   F32 value                            //!< Value to set global to
                                   ) override;

    //! Handler implementation for command GLOBAL_SET_F64
    //!
    //! Set a global variable to a given f64 value.
    //! Command fails if the module is not found, global is not exported, mutable, or of f64 type
    void GLOBAL_SET_F64_cmdHandler(FwOpcodeType opCode,                 //!< The opcode
                                   U32 cmdSeq,                          //!< The command sequence number
                                   const Fw::CmdStringArg& moduleName,  //!< Name of the module to set global for
                                   const Fw::CmdStringArg& name,        //!< Name of the global
                                   F64 value                            //!< Value to set global to
                                   ) override;

    //! Handler implementation for command GLOBAL_GET
    //!
    //! Get the current value of a global variable and emit an event
    //! Command fails if the module is not found or the global is not exported
    void GLOBAL_GET_cmdHandler(FwOpcodeType opCode,                 //!< The opcode
                               U32 cmdSeq,                          //!< The command sequence number
                               const Fw::CmdStringArg& moduleName,  //!< Name of the module to get global for
                               const Fw::CmdStringArg& name         //!< Name of the global
                               ) override;

  private:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine actions
    // ----------------------------------------------------------------------

    //! Implementation for action processInvoke of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Action to save the invoke arguments and emit an 'invoked' signal
    void Svc_WasmSequencer_ControllerStateMachine_action_processInvoke(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_InvokeRequest& value             //!< The value
        ) override;

    //! Implementation for action setCancelRequested of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Latch a cancel that arrives while a sequence is loading/resolving
    void Svc_WasmSequencer_ControllerStateMachine_action_setCancelRequested(
        SmId smId,                                               //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action clearCancelRequested of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Acknowledge/clear any pending cancel
    void Svc_WasmSequencer_ControllerStateMachine_action_clearCancelRequested(
        SmId smId,                                               //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action cancelPendingRequest of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Abort a load/invoke that was cancelled before the engine ran
    void Svc_WasmSequencer_ControllerStateMachine_action_cancelPendingRequest(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value            //!< The value
        ) override;

    //! Implementation for action respond_noblock_OK of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Responds to $block == NO_BLOCK requests with OK
    void Svc_WasmSequencer_ControllerStateMachine_action_respond_noblock_OK(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value            //!< The value
        ) override;

    //! Implementation for action respond_ERROR of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Responds to request with EXECUTION_ERROR
    void Svc_WasmSequencer_ControllerStateMachine_action_respond_ERROR(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value            //!< The value
        ) override;

    //! Implementation for action incrementSequenceFailure of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Increment the SequencesFailed telemetry counter
    void Svc_WasmSequencer_ControllerStateMachine_action_incrementSequenceFailure(
        SmId smId,                                               //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action respondInvoke_BUSY of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Responds to request with BUSY
    //! Emit an event to say why we are rejecting this request in the current state
    void Svc_WasmSequencer_ControllerStateMachine_action_respondInvoke_BUSY(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_InvokeRequest& value             //!< The value
        ) override;

    //! Implementation for action respondInvoke_ERROR of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Responds to request with EXECUTION_ERROR
    //! Emit an event to say why we are rejecting this request in the current state
    void Svc_WasmSequencer_ControllerStateMachine_action_respondInvoke_ERROR(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_InvokeRequest& value             //!< The value
        ) override;

    //! Implementation for action respondLoad_BUSY of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Responds to request with BUSY
    //! Emit an event to say why we are rejecting this request in the current state
    void Svc_WasmSequencer_ControllerStateMachine_action_respondLoad_BUSY(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_LoadRequest& value               //!< The value
        ) override;

    //! Implementation for action respond_block_OK of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Responds to $block == BLOCK requests with OK
    //! Responds to any pending WAIT requests with OK
    void Svc_WasmSequencer_ControllerStateMachine_action_respond_block_OK(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value            //!< The value
        ) override;

    //! Implementation for action respond_block_ERROR of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Responds to $block == BLOCK requests with ERROR.
    //! Responds to any pending WAIT requests with ERROR
    void Svc_WasmSequencer_ControllerStateMachine_action_respond_block_ERROR(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value            //!< The value
        ) override;

    //! Implementation for action load of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Load a module into the store.
    //! Emits `loadSucceded` or `loadFailed` depending on result
    void Svc_WasmSequencer_ControllerStateMachine_action_load(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_LoadRequest& value               //!< The value
        ) override;

    //! Implementation for action invokeStart of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Invoke the start function on a loaded module
    void Svc_WasmSequencer_ControllerStateMachine_action_invokeStart(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value            //!< The value
        ) override;

    //! Implementation for action invokeMain of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Invoke the main function for module in context
    void Svc_WasmSequencer_ControllerStateMachine_action_invokeMain(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value            //!< The value
        ) override;

    //! Implementation for action reportModuleInvalidMain of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Emit an event noting that a given module has no [valid] main function
    void Svc_WasmSequencer_ControllerStateMachine_action_reportModuleInvalidMain(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value            //!< The value
        ) override;

    //! Implementation for action reportModuleMainInvokeFailed of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Emit an event noting why the invocation of a module main failed
    void Svc_WasmSequencer_ControllerStateMachine_action_reportModuleMainInvokeFailed(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value            //!< The value
        ) override;

    //! Implementation for action reportModuleStartInvokeFailed of state machine
    //! Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Emit an event noting why the invocation of a module start failed
    void Svc_WasmSequencer_ControllerStateMachine_action_reportModuleStartInvokeFailed(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value            //!< The value
        ) override;

    //! Implementation for action reportModuleStarted of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Reports that a module started running it's main function
    void Svc_WasmSequencer_ControllerStateMachine_action_reportModuleStarted(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value            //!< The value
        ) override;

    //! Implementation for action reportModuleSucceeded of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Emit an event noting a module succeeded during execution. Increment telemetry counters.
    void Svc_WasmSequencer_ControllerStateMachine_action_reportModuleSucceeded(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value            //!< The value
        ) override;

    //! Implementation for action reportModuleStartFailed of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Emit an event noting a module failed during execution of start. Increment telemetry counters.
    void Svc_WasmSequencer_ControllerStateMachine_action_reportModuleStartFailed(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value            //!< The value
        ) override;

    //! Implementation for action reportModuleMainFailed of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Emit an event noting a module failed during execution. Increment telemetry counters.
    void Svc_WasmSequencer_ControllerStateMachine_action_reportModuleMainFailed(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value            //!< The value
        ) override;

    //! Implementation for action resetStore of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Create a new store with N_MODULES (parameter) modules
    void Svc_WasmSequencer_ControllerStateMachine_action_resetStore(
        SmId smId,                                               //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action runEngine of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Send a signal to the engine state machine to begin running
    //! The engine will asynchronously reply with the `engineFinished` signal
    void Svc_WasmSequencer_ControllerStateMachine_action_runEngine(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value            //!< The value
        ) override;

    //! Implementation for action cmdReplyOK of state machine Svc_WasmSequencer_InterpreterStateMachine
    //!
    //! Respond to a pending command with OK
    void Svc_WasmSequencer_InterpreterStateMachine_action_cmdReplyOK(
        SmId smId,                                                 //!< The state machine id
        Svc_WasmSequencer_InterpreterStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_CommandRequest& value             //!< The value
        ) override;

    //! Implementation for action signalEntered of state machine Svc_WasmSequencer_InterpreterStateMachine
    //!
    //! generic signal raised
    void Svc_WasmSequencer_InterpreterStateMachine_action_signalEntered(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_InterpreterStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action spin of state machine Svc_WasmSequencer_InterpreterStateMachine
    //!
    //! spins the interpreter loop, executing up to a bounded number of instructions
    void Svc_WasmSequencer_InterpreterStateMachine_action_spin(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_InterpreterStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action reset of state machine Svc_WasmSequencer_InterpreterStateMachine
    //!
    //! resets the engine's state (clears operand stack, pc, fp, sp)
    void Svc_WasmSequencer_InterpreterStateMachine_action_reset(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_InterpreterStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action clearExitStatus of state machine Svc_WasmSequencer_InterpreterStateMachine
    void Svc_WasmSequencer_InterpreterStateMachine_action_clearExitStatus(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_InterpreterStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action setExitReason_INTERPRETER_FINISHED of state machine
    //! Svc_WasmSequencer_InterpreterStateMachine
    void Svc_WasmSequencer_InterpreterStateMachine_action_setExitReason_INTERPRETER_FINISHED(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_InterpreterStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action setExitReason_INTERPRETER_TRAP of state machine
    //! Svc_WasmSequencer_InterpreterStateMachine
    void Svc_WasmSequencer_InterpreterStateMachine_action_setExitReason_INTERPRETER_TRAP(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_InterpreterStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action setExitReason_REPLY_TIMEOUT of state machine Svc_WasmSequencer_InterpreterStateMachine
    void Svc_WasmSequencer_InterpreterStateMachine_action_setExitReason_REPLY_TIMEOUT(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_InterpreterStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action setExitReason_HOST_FAILURE of state machine Svc_WasmSequencer_InterpreterStateMachine
    void Svc_WasmSequencer_InterpreterStateMachine_action_setExitReason_HOST_FAILURE(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_InterpreterStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action setExitReason_TIMER_INCOMPARABLE of state machine
    //! Svc_WasmSequencer_InterpreterStateMachine
    void Svc_WasmSequencer_InterpreterStateMachine_action_setExitReason_TIMER_INCOMPARABLE(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_InterpreterStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action setExitReason_UNEXPECTED_REPLY of state machine
    //! Svc_WasmSequencer_InterpreterStateMachine
    void Svc_WasmSequencer_InterpreterStateMachine_action_setExitReason_UNEXPECTED_REPLY(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_InterpreterStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action setExitReason_CANCEL of state machine Svc_WasmSequencer_InterpreterStateMachine
    void Svc_WasmSequencer_InterpreterStateMachine_action_setExitReason_CANCEL(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_InterpreterStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action setExitCode of state machine Svc_WasmSequencer_InterpreterStateMachine
    void Svc_WasmSequencer_InterpreterStateMachine_action_setExitCode(
        SmId smId,                                                 //!< The state machine id
        Svc_WasmSequencer_InterpreterStateMachine::Signal signal,  //!< The signal
        I32 value                                                  //!< The value
        ) override;

    //! Implementation for action setTrapReason of state machine Svc_WasmSequencer_InterpreterStateMachine
    void Svc_WasmSequencer_InterpreterStateMachine_action_setTrapReason(
        SmId smId,                                                 //!< The state machine id
        Svc_WasmSequencer_InterpreterStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_TrapReason& value                 //!< The value
        ) override;

    //! Implementation for action setLastHostFunction of state machine Svc_WasmSequencer_InterpreterStateMachine
    void Svc_WasmSequencer_InterpreterStateMachine_action_setLastHostFunction(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_InterpreterStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action finish of state machine Svc_WasmSequencer_InterpreterStateMachine
    //!
    //! Send a signal back to the controller state machine that we have finished executing
    //! The response codes are stored in m_exit (reason, code, lastTrapReason)
    void Svc_WasmSequencer_InterpreterStateMachine_action_finish(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_InterpreterStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action reportPaused of state machine Svc_WasmSequencer_InterpreterStateMachine
    //!
    //! reports that execution was paused at a breakpoint
    void Svc_WasmSequencer_InterpreterStateMachine_action_reportPaused(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_InterpreterStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action clearPause of state machine Svc_WasmSequencer_InterpreterStateMachine
    //!
    //! sets the pause flag to false
    void Svc_WasmSequencer_InterpreterStateMachine_action_clearPause(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_InterpreterStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action dispatchPendingHostFunction of state machine Svc_WasmSequencer_InterpreterStateMachine
    //!
    //! dispatch a host function port call
    void Svc_WasmSequencer_InterpreterStateMachine_action_dispatchPendingHostFunction(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_InterpreterStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action clearPendingHostFunction of state machine Svc_WasmSequencer_InterpreterStateMachine
    //!
    //! clears the pending host function port call
    void Svc_WasmSequencer_InterpreterStateMachine_action_clearPendingHostFunction(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_InterpreterStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action setContext of state machine Svc_WasmSequencer_InterpreterStateMachine
    //!
    //! Set the current executing context
    void Svc_WasmSequencer_InterpreterStateMachine_action_setContext(
        SmId smId,                                                 //!< The state machine id
        Svc_WasmSequencer_InterpreterStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value             //!< The value
        ) override;

    //! Implementation for action clearContext of state machine Svc_WasmSequencer_InterpreterStateMachine
    //!
    //! Clear the current executing context
    void Svc_WasmSequencer_InterpreterStateMachine_action_clearContext(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_InterpreterStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action resume of state machine Svc_WasmSequencer_InterpreterStateMachine
    //!
    //! spacewasm_engine_resume
    void Svc_WasmSequencer_InterpreterStateMachine_action_resume(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_InterpreterStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action resumeI32 of state machine Svc_WasmSequencer_InterpreterStateMachine
    //!
    //! spacewasm_engine_resume_some(I32(value))
    void Svc_WasmSequencer_InterpreterStateMachine_action_resumeI32(
        SmId smId,                                                 //!< The state machine id
        Svc_WasmSequencer_InterpreterStateMachine::Signal signal,  //!< The signal
        I32 value                                                  //!< The value
        ) override;

    //! Implementation for action checkSleepTimers of state machine Svc_WasmSequencer_InterpreterStateMachine
    //!
    //! A periodic check on the pending timer to see if we can wake up
    void Svc_WasmSequencer_InterpreterStateMachine_action_checkSleepTimers(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_InterpreterStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action checkTimeout of state machine Svc_WasmSequencer_InterpreterStateMachine
    //!
    //! A periodic check on any host function to guard against timeouts
    void Svc_WasmSequencer_InterpreterStateMachine_action_checkTimeout(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_InterpreterStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action dequeueSerialAndResume of state machine Svc_WasmSequencer_InterpreterStateMachine
    //!
    //! Dequeue a serial message into the host guest memory and resume the interpreter
    void Svc_WasmSequencer_InterpreterStateMachine_action_dequeueSerialAndResume(
        SmId smId,                                                 //!< The state machine id
        Svc_WasmSequencer_InterpreterStateMachine::Signal signal,  //!< The signal
        const FwIndexType& value                                   //!< The value
        ) override;

  private:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine guards
    // ----------------------------------------------------------------------

    //! Implementation for guard cancelRequested of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! True if a cancel was latched since the controller last came to rest.
    bool Svc_WasmSequencer_ControllerStateMachine_guard_cancelRequested(
        SmId smId,                                               //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal  //!< The signal
    ) const override;

    //! Implementation for guard moduleHasStart of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! return true if this module has a start function
    bool Svc_WasmSequencer_ControllerStateMachine_guard_moduleHasStart(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value            //!< The value
    ) const override;

    //! Implementation for guard moduleHasValidMain of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! return true if this module has a valid main function ([] -> i32)
    bool Svc_WasmSequencer_ControllerStateMachine_guard_moduleHasValidMain(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value            //!< The value
    ) const override;

    //! Implementation for guard invokeSucceeded of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! return true if invokeStatus == SPACEWASM_OK. This flag is set as a result of invokeStart/invokeMain
    bool Svc_WasmSequencer_ControllerStateMachine_guard_invokeSucceeded(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value            //!< The value
    ) const override;

    //! Implementation for guard interpreterSucceeded of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Check if the last engine execution finished executing successfully
    bool Svc_WasmSequencer_ControllerStateMachine_guard_interpreterSucceeded(
        SmId smId,                                               //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal  //!< The signal
    ) const override;

    //! Implementation for guard pendingPause of state machine Svc_WasmSequencer_InterpreterStateMachine
    //!
    //! return true if execution should pause before spinning the interpreter again
    bool Svc_WasmSequencer_InterpreterStateMachine_guard_pendingPause(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_InterpreterStateMachine::Signal signal  //!< The signal
    ) const override;

    //! Implementation for guard pendingHostFunction of state machine Svc_WasmSequencer_InterpreterStateMachine
    //!
    //! a host function is waiting to be processed
    bool Svc_WasmSequencer_InterpreterStateMachine_guard_pendingHostFunction(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_InterpreterStateMachine::Signal signal  //!< The signal
    ) const override;

    //! Implementation for guard pendingHostFunctionIsSleep of state machine Svc_WasmSequencer_InterpreterStateMachine
    //!
    //! the pending host function is a sleep (therefore we need to check the sleep timers)
    bool Svc_WasmSequencer_InterpreterStateMachine_guard_pendingHostFunctionIsSleep(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_InterpreterStateMachine::Signal signal  //!< The signal
    ) const override;

    //! Implementation for guard blockingSerialIn of state machine Svc_WasmSequencer_InterpreterStateMachine
    //!
    //! Check if we are currently blocking on a serial_recv() for a given serial port index
    bool Svc_WasmSequencer_InterpreterStateMachine_guard_blockingSerialIn(
        SmId smId,                                                 //!< The state machine id
        Svc_WasmSequencer_InterpreterStateMachine::Signal signal,  //!< The signal
        const FwIndexType& value                                   //!< The value
    ) const override;

    //! Implementation for guard dequeueSucceeded of state machine Svc_WasmSequencer_InterpreterStateMachine
    //!
    //! Return true `dequeueSerialAndResume` processed a message successfully
    bool Svc_WasmSequencer_InterpreterStateMachine_guard_dequeueSucceeded(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_InterpreterStateMachine::Signal signal  //!< The signal
    ) const override;

  private:
    /// The global allocator callbacks
    U8* globalAlloc(U32 size, U32 align);
    void globalDealloc(const U8* ptr);

    /// C-callback trampolines for the spacewasm global-allocator registry
    static U8* globalAllocCallback(void* userdata, size_t size, size_t align);
    static void globalDeallocCallback(void* userdata, U8* ptr, size_t size, size_t align);

    /// The Wasm guest allocator callbacks for this component
    U8* guestAlloc(FwSizeType size, U32 align);
    U8* guestRealloc(U8* ptr, FwSizeType oldSize, FwSizeType newSize, U32 align);
    void guestDealloc(const U8* ptr, FwSizeType size);

    // Guest allocator callbacks passed to C API
    static U8* guestAllocCallback(void* userdata, size_t size, size_t align);
    static U8* guestReallocCallback(void* userdata, U8* ptr, size_t old_size, size_t new_size, size_t align);
    static void guestDeallocCallback(void* userdata, U8* ptr, size_t size, size_t align);

    //! Create a fresh interpreter store with the given module capacity,
    //! destroying any existing store first.
    void createStore();

    //! Destroy the current interpreter store, if any, releasing its memory.
    void destroyStore();

    //! Take control of the spacewasm global allocator on this WasmSequence
    void takeAllocatorLock();

    //! Release control of the spacewasm global allocator on this WasmSequence
    void releaseAllocatorLock();

    //! Map a spacewasm_trap_t onto the TrapReason event enum.
    static Svc::WasmSequencer_TrapReason::T mapTrapReason(spacewasm_trap_t trap);

    //! Record the SeqName telemetry for a load. Uses moduleName when non-empty;
    //! otherwise derives it from the file's basename with any ".wasm" suffix
    //! stripped (an empty-name RUN / LOAD).
    void setSequenceName(const Fw::StringBase& filePath, const Fw::StringBase& moduleName);

    //! Return a pointer to the basename of `path` (the segment after the last '/')
    //! and write its length to `outLen`. `len` is the length of `path`. Pure string
    //! manipulation, no filesystem access.
    static const char* pathBaseName(const char* path, FwSizeType len, FwSizeType& outLen);

    //! True if `path` contains a ".." path-traversal component (a segment, delimited
    //! by '/', that is exactly ".."). Used to keep a ground-supplied sequence file
    //! name from escaping the configured SEQ_BASE_DIR.
    static bool pathHasParentTraversal(const Fw::StringBase& path);

    //! Resolve a sequence `fileName` against the SEQ_BASE_DIR parameter, writing the
    //! result to `filePath`. On failure (a ".." component that would escape the base
    //! dir, or a joined path that overflows the buffer) it logs the specific event
    //! and returns false; the caller is responsible for failing the load.
    bool resolveSequencePath(const Fw::StringBase& fileName, Fw::String& filePath);

    //! Read `len` bytes of guest linear memory at `addr` into `dst` (via
    //! spacewasm_mem_read on the pending host function's caller).
    Fw::Success readGuestMemory(WasmSequencer_HostFunction::T kind, U32 addr, U8* dst, FwSizeType len);

    //! Write `len` bytes from `src` into guest linear memory at `addr` (via
    //! spacewasm_mem_write)
    Fw::Success writeGuestMemory(WasmSequencer_HostFunction::T kind, U32 addr, const U8* src, FwSizeType len);

    Fw::MemAllocator* m_allocator = nullptr;

    //! Memory-resource configuration captured by configure()
    Config m_config;

    //! Page pool backing the process-wide spacewasm global page allocator for this instance.
    //! Each page is `Svc::WasmSequencerConfig::SPACEWASM_PAGE_SIZE` with m_config.heapPages pages.
    //! Stores dynamic memory allocated to hold each loaded module in the store (and the store itself).
    U8** m_heapPages;

    //! Number of currently used
    FwSizeType m_heapPagesUsed;

    //! A deallocation has poisoned the heap allocator
    //! No more allocations can happen until every page is dropped.
    //! SpaceWasm will drop everything in one shot so this simply helps us guard this invariant with an assertion
    bool m_heapPoisoned;

    //! Pool backing the per-load guest linear-memory allocator; a simple
    //! bump allocator (guest modules are compiled with memory.grow disabled).
    U8* m_guestPool;

    //! Current bump offset into `m_guestPool`.
    FwSizeType m_guestPoolOffset;

    //! Opaque handle to the spacewasm engine, or null (before the store is initialized).
    spacewasm_t* m_wasm;

    //! Pending command waiting for a response
    struct WaitingCmd {
        FwOpcodeType opCode;
        U32 cmdSeq;

        WaitingCmd() : opCode(0), cmdSeq(0) {}
        WaitingCmd(FwOpcodeType opCode_, U32 cmdSeq_) : opCode(opCode_), cmdSeq(cmdSeq_) {}
    };

    //! WAIT commands waiting for sequence completion
    Fw::FifoQueue<WaitingCmd, WasmSequencerConfig::MAX_CONCURRENT_WAIT_COMMANDS> m_waiting;

    //! Currently stored sequence arguments
    Svc::SeqArgs m_args;

    //! File path of the last module load
    Fw::FileNameString m_lastLoadFileName;

    WasmSequencer_RequestContext m_executingContext;
    bool m_hasExecutingContext;

    //! Pending timer from sleep host function
    Fw::Time m_pendingTimer;
    bool m_hasPendingTimer;

    //! Wall-clock time at which the current blocking async host function
    //! (COMMAND / blocking SERIAL_RECV) began awaiting its reply. Used to enforce
    //! HOST_FUNCTION_TIMEOUT_SECS. Only meaningful while awaiting one of those.
    Fw::Time m_hostFunctionStart;
    bool m_hasHostFunctionStart;

    bool m_dequeueSucceeded;

    //! Flag indicating a function invocation failed
    spacewasm_status_t m_invokeStatus;

    //! Flag indicating interpreter is waiting to be paused
    bool m_pendingPause;

    //! Latches a CANCEL that arrives while a sequence is loading/resolving
    bool m_cancelRequested;

    //! Backing state for the periodically-written telemetry channels (see
    //! writeTelemetry_handler). Grouped so the counters read as one unit.
    struct Telemetry {
        //! Sequences successfully completed
        U64 sequencesSucceeded{0};

        //! Sequences that failed to validate or execute
        U64 sequencesFailed{0};

        //! Sequences that were cancelled
        U64 sequencesCancelled{0};

        //! Commands dispatched total
        U64 commandsDispatched{0};

        //! Number of commands that failed
        U64 commandsFailed{0};

        //! Currently running sequence name
        Fw::FileNameString sequenceName{""};
    };

    Telemetry m_tlm;

    //! Sequences started counter (bumped whenever the interpreter
    //! begins spinning a freshly-invoked program). The low 16 bits form the
    //! high half of the command context (cmdUid) so we can detect a command
    //! response that arrives late, after its originating sequence has ended.
    U32 m_sequencesStarted;

    //! Compose the command context (cmdUid) sent to the command dispatcher from
    //! the current sequence counter and the m_tlm.commandsDispatched counter. The
    //! low 16 bits of the latter form the low half of the cmdUid, letting us
    //! detect a response for a different instance of the same opcode.
    U32 makeCmdUid() const;

    //! Why the last sequence ended. Set as the program runs and classified into
    //! the appropriate completion event (see reportSequenceFailure in the controller).
    struct ExitStatus {
        //! Reason the current program exited.
        //! By default this is INTERPRETER but can be overriden from host functions
        WasmSequencer_ExitReason reason{WasmSequencer_ExitReason::UNKNOWN};

        //! Currently stored exit code for non-INTERPRETER exits
        I32 code{0};

        //! Last run host function that could have caused the error
        WasmSequencer_HostFunction lastHostFunction{WasmSequencer_HostFunction::NONE};

        //! Reason last sequence trapped
        WasmSequencer_TrapReason lastTrapReason{WasmSequencer_TrapReason::NONE};
    };

    //! Status codes for exit reason. Set by the interpreter state machine.
    ExitStatus m_exit;

    //! Buffer to hold the serial output port invocation invoked by the guest
    Fw::ExternalSerializeBuffer m_serialOutBuffer;

    //! Queues (or queue) that handle inputs on the serial input port. Each is backed by
    //! the corresponding row of m_serialInQueueData (see the setup() loop in the ctor).
    Types::CircularBuffer m_serialInQueue[NUM_SERIALIN_INPUT_PORTS];

    //! A lock for guarding the serialInQueue
    Os::Mutex m_serialInMutex;

    //! A host function call the guest requested that is pending dispatch by the
    //! engine state machine (see dispatchPendingHostFunction). `kind` selects
    //! which arm of the `u` union carries the call's arguments.
    struct PendingHostFunction {
        PendingHostFunction() = default;
        bool isPending() const { return kind != WasmSequencer_HostFunction::NONE; }
        void clear() { kind = WasmSequencer_HostFunction::NONE; }

        WasmSequencer_HostFunction kind{WasmSequencer_HostFunction::NONE};

        // Handle that holds the Wasm guest memory pointer
        spacewasm_caller_t* caller{nullptr};

        //! Per-kind call arguments. Only the arm matching `kind` is live.
        union Args {
            // COMMAND: encoded command payload in guest memory
            struct {
                U32 ptr;
                U32 len;
            } command;

            // TELEMETRY: channel id plus where to write the serialized time and value
            struct {
                FwChanIdType chanId;
                U32 timePtr;
                U32 timeLen;
                U32 valuePtr;
                U32 valueLen;
            } telemetry;

            // PARAMETER: parameter id plus where to write the serialized value
            struct {
                FwPrmIdType prmId;
                U32 ptr;
                U32 len;
            } parameter;

            // EVENT: raw guest-requested severity (may be out of range) plus the message
            struct {
                U32 rawSeverity;
                U32 msgPtr;
                U32 msgLen;
            } event;

            // RSLEEP: relative sleep duration
            struct {
                U64 us;
            } rsleep;

            // ASLEEP: absolute sleep time
            struct {
                U64 us;
            } asleep;

            // ARGS: where to write the stored sequence arguments
            struct {
                U32 ptr;
                U32 len;
            } args;

            // TIME: where to write the serialized current time
            struct {
                U32 ptr;
                U32 len;
            } time;

            // SERIAL_OUT: serial output port index plus payload in guest memory
            struct {
                U32 index;
                U32 ptr;
                U32 len;
            } serialOut;

            // SERIAL_RECV: Read a message from the serialIn port queue given a port index
            struct {
                U32 index;
                U32 dataPtr;
                U32 dataSize;
                U32 actualSizePtr;
                Svc::BlockState::T blockingType;
            } serialRecv;

            Args() : command{0, 0} {}
        } u;
    };

    PendingHostFunction m_pendingHostFunction;

    //! Helper function for checking the signature of a modules main function
    spacewasm_status_t validateModuleMain(WasmSequencer_ModuleIdx moduleIdx) const;

    //! Report an engine-completion failure as the appropriate distinct event
    //! (SequenceExited / SequencePanic / SequenceTrapped / SequenceHostFailure,
    //! or SequenceCancelled) and bump the matching telemetry counter. `phase`
    //! records whether the failure occurred running the module's start function
    //! or its main entrypoint.
    void reportSequenceRuntimeFailure(WasmSequencer_ModuleIdx moduleIdx, WasmSequencer_SequencePhase phase);

    //! Respond to a request with certain reply
    void respondToRequest(const Svc::WasmSequencer_RequestContext& value, const Fw::CmdResponse& response);

    //! Send response to all waiting requests
    void respondToWaiting(const Fw::CmdResponse& response);

    //! Report that a RUN finished on seqDoneOut (if connected) with the given
    //! response. Only emits for RUN-sourced requests, matching the RUN-gated
    //! seqStartOut in reportModuleStarted, so the seqStart/seqDone pair stays
    //! balanced. A no-op for INVOKE/LOAD, so both controller completion actions
    //! (respond_block_OK and respond_block_ERROR, which send the final command
    //! response) can call it unconditionally.
    void reportSeqDone(const Svc::WasmSequencer_RequestContext& value, const Fw::CmdResponse& response);

    //! Set a global to a value given the name of the module, global export name and value
    spacewasm_status_t setGlobal(const Fw::StringBase& moduleName, const Fw::StringBase& name, spacewasm_value_t value);

    //! Get the value of a global variable given its module name and global name
    spacewasm_status_t getGlobal(const Fw::StringBase& moduleName,
                                 const Fw::StringBase& name,
                                 spacewasm_value_t& value);

    //! Set up the fprime host interface
    void hostFprimeV1(spacewasm_host_t*);

    /// FPrime Wasm Interface Host Functions
    spacewasm_hostcall_result_t wasmExit(struct spacewasm_caller_t* caller,
                                         const struct spacewasm_value_t* params,
                                         size_t n_params,
                                         struct spacewasm_value_t* out_result);

    spacewasm_hostcall_result_t wasmPanic(struct spacewasm_caller_t* caller,
                                          const struct spacewasm_value_t* params,
                                          size_t n_params,
                                          struct spacewasm_value_t* out_result);

    spacewasm_hostcall_result_t wasmArgs(struct spacewasm_caller_t* caller,
                                         const struct spacewasm_value_t* params,
                                         size_t n_params,
                                         struct spacewasm_value_t* out_result);

    spacewasm_hostcall_result_t wasmTime(struct spacewasm_caller_t* caller,
                                         const struct spacewasm_value_t* params,
                                         size_t n_params,
                                         struct spacewasm_value_t* out_result);

    spacewasm_hostcall_result_t wasmReadTelemetry(struct spacewasm_caller_t* caller,
                                                  const struct spacewasm_value_t* params,
                                                  size_t n_params,
                                                  struct spacewasm_value_t* out_result);

    spacewasm_hostcall_result_t wasmReadParameter(struct spacewasm_caller_t* caller,
                                                  const struct spacewasm_value_t* params,
                                                  size_t n_params,
                                                  struct spacewasm_value_t* out_result);

    spacewasm_hostcall_result_t wasmCommand(struct spacewasm_caller_t* caller,
                                            const struct spacewasm_value_t* params,
                                            size_t n_params,
                                            struct spacewasm_value_t* out_result);

    spacewasm_hostcall_result_t wasmEvent(struct spacewasm_caller_t* caller,
                                          const struct spacewasm_value_t* params,
                                          size_t n_params,
                                          struct spacewasm_value_t* out_result);

    spacewasm_hostcall_result_t wasmRsleep(struct spacewasm_caller_t* caller,
                                           const struct spacewasm_value_t* params,
                                           size_t n_params,
                                           struct spacewasm_value_t* out_result);

    spacewasm_hostcall_result_t wasmAsleep(struct spacewasm_caller_t* caller,
                                           const struct spacewasm_value_t* params,
                                           size_t n_params,
                                           struct spacewasm_value_t* out_result);

    spacewasm_hostcall_result_t wasmSerialOut(struct spacewasm_caller_t* caller,
                                              const struct spacewasm_value_t* params,
                                              size_t n_params,
                                              struct spacewasm_value_t* out_result);

    spacewasm_hostcall_result_t wasmSerialRecv(struct spacewasm_caller_t* caller,
                                               const struct spacewasm_value_t* params,
                                               size_t n_params,
                                               struct spacewasm_value_t* out_result);

    //! COMMAND: forward an encoded command from guest memory to the command dispatcher.
    void dispatchCommand();

    //! TELEMETRY: read a telemetry channel and write its value + time into guest memory.
    void dispatchTelemetry();

    //! PARAMETER: read a parameter and write it into guest memory.
    void dispatchParameter();

    //! EVENT: emit a guest-requested event at the guest-requested severity.
    void dispatchEvent();

    //! RSLEEP: arm a relative sleep timer.
    void dispatchRelativeSleep();

    //! ASLEEP: arm an absolute sleep timer.
    void dispatchAbsoluteSleep();

    //! ARGS: write the stored sequence arguments into guest memory.
    void dispatchArgs();

    //! TIME: write the current time into guest memory.
    void dispatchTime();

    //! SERIAL_OUT: copy a payload out of guest memory and invoke the serial output port.
    void dispatchSerialOut();

    //! SERIAL_RECV: check the serial input queue and either resume or block awaiting a message.
    void dispatchSerialRecv();

    // A global static lock. This is needed to allow the global allocator in spacewasm
    // to not require to pass context to fine grained context to allocations.
    // Read more about this in the SDD.
    static Os::Mutex* getGlobalAllocatorLock();
};

}  // namespace Svc

#endif
