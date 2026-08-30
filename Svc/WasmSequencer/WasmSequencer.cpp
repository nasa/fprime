// ======================================================================
// \title  WasmSequencer.cpp
// \author tumbar
// \brief  cpp file for WasmSequencer component implementation class
// ======================================================================

#include "Svc/WasmSequencer/WasmSequencer.hpp"

#include "Fw/Cmd/CmdResponseEnumAc.hpp"
#include "Fw/Types/Assert.hpp"
#include "Fw/Types/LinearBufferTemplate.hpp"
#include "Fw/Types/Serializable.hpp"
#include "Fw/Types/SuccessEnumAc.hpp"
#include "Os/Mutex.hpp"
#include "Svc/Seq/BlockStateEnumAc.hpp"
#include "Svc/Seq/SeqArgsSerializableAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_CommandRequestSerializableAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_ControllerStateMachine_StateEnumAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_HostFunctionEnumAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_InvokeRequestSerializableAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_LoadRequestSerializableAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_SignalSourceEnumAc.hpp"
#include "Svc/WasmSequencer/fprime_spacewasm/include/fprime_spacewasm.h"
#include "Svc/WasmSequencer/spacewasm_include/spacewasm.h"
#include "config/FwAssertArgTypeAliasAc.h"
#include "config/FwSizeTypeAliasAc.h"
#include "config/WasmSequencerSpacewasmConfig.h"

namespace Svc {

U8* WasmSequencer ::globalAllocCallback(void* userdata, size_t size, size_t align) {
    if (userdata == nullptr) {
        return nullptr;
    }
    return static_cast<WasmSequencer*>(userdata)->globalAlloc(static_cast<U32>(size), static_cast<U32>(align));
}

void WasmSequencer ::globalDeallocCallback(void* userdata, U8* ptr, size_t size, size_t align) {
    (void)size;
    (void)align;
    if (userdata != nullptr) {
        static_cast<WasmSequencer*>(userdata)->globalDealloc(ptr);
    }
}

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

WasmSequencer ::WasmSequencer(const char* const compName)
    : WasmSequencerComponentBase(compName),
      m_guestPoolOffset(0),
      m_wasm(nullptr),
      m_hasExecutingContext(false),
      m_pendingTimer(),
      m_hasPendingTimer(false),
      m_hostFunctionStart(),
      m_hasHostFunctionStart(false),
      m_dequeueSucceeded(false),
      m_invokeStatus(SPACEWASM_OK),
      m_pendingPause(false),
      m_cancelRequested(false),
      m_sequencesStarted(0),
      m_serialInFullBehavior{SerialInQueueFullBehavior::DROP_NEWEST} {
    getGlobalAllocatorLock()->lock();
    const auto status = spacewasm_fprime_register_global_allocator(&globalAllocCallback, &globalDeallocCallback, this);
    getGlobalAllocatorLock()->unlock();

    FW_ASSERT(status == SPACEWASM_OK, status);
}

WasmSequencer ::~WasmSequencer() {
    this->destroyStore();

    // Release our slot in the process-wide global-allocator registry so it can
    // be reused by a later sequencer instance.
    getGlobalAllocatorLock()->lock();
    (void)spacewasm_fprime_deregister_global_allocator(this);
    getGlobalAllocatorLock()->unlock();
}

void WasmSequencer ::configure(FwSizeType dynamicMemPageCount,
                               FwSizeType wasmGuestMemorySize,
                               FwSizeType wasmStackSize,
                               FwSizeType serialOutMaxSize,
                               SerialInQueueConfig serialInQueueCfg,
                               Fw::MemAllocator& mallocator) {
    // Allocate the dynamic memory pool
    {
        auto actualSize = dynamicMemPageCount * Svc::WasmSequencerConfig::SPACEWASM_PAGE_SIZE;
        auto ptr = mallocator.allocate(0, actualSize, SPACEWASM_MEMORY_ALIGNMENT);
        FW_ASSERT(this->m_dynamicPool != nullptr, static_cast<FwAssertArgType>(dynamicMemPageCount),
                  WASM_SEQ_SPACEWASM_PAGE_SIZE);

        this->m_dynamicPagesUsed = 0;
        this->m_dynamicPoolPageCount = dynamicMemPageCount;
        this->m_dynamicPool = reinterpret_cast<U8*>(ptr);
    }

    // Allocate the guest memory pool
    {
        auto actualSize = wasmGuestMemorySize;
        auto ptr = mallocator.allocate(1, actualSize, SPACEWASM_MEMORY_ALIGNMENT);
        FW_ASSERT(this->m_guestPool != nullptr, static_cast<FwAssertArgType>(wasmGuestMemorySize));

        this->m_guestPoolOffset = 0;
        this->m_guestPoolSize = actualSize;
        this->m_guestPool = reinterpret_cast<U8*>(ptr);
    }

    // The Wasm stack is allocated into the dynamic pool during store initialization
    this->m_wasmStackSize = wasmStackSize;

    // Allocate the serialOut buffer
    if (serialOutMaxSize > 0) {
        auto actualSize = serialOutMaxSize;
        auto ptr = mallocator.allocate(2, actualSize);
        FW_ASSERT(ptr != nullptr, static_cast<FwAssertArgType>(serialOutMaxSize));

        this->m_serialOutBuffer.setExtBuffer(reinterpret_cast<U8*>(ptr), actualSize);
    }

    // Allocate the serialIn queues
    {
        Os::ScopeLock scopeLock(this->m_serialInMutex);
        for (FwIndexType i = 0; i < NUM_SERIALIN_INPUT_PORTS; i++) {
            if (serialInQueueCfg.sizes[i] > 0) {
                auto actualSize = serialInQueueCfg.sizes[i];
                auto ptr = mallocator.allocate(3 + i, actualSize);
                FW_ASSERT(ptr != nullptr, i, static_cast<FwAssertArgType>(serialInQueueCfg.sizes[i]));

                this->m_serialInQueue[i].setup(reinterpret_cast<U8*>(ptr), actualSize);
                this->m_serialInFullBehavior[i] = serialInQueueCfg.fullBehavior[i];
            } else {
                // Queue is zero size, always drop
                this->m_serialInFullBehavior[i] = SerialInQueueFullBehavior::DROP_NEWEST;
            }
        }
    }
}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void WasmSequencer ::checkTimers_handler(FwIndexType portNum, U32 context) {
    // Drive the sleep-wake / host-function-timeout checks in the state machine.
    this->interpreter_sendSignal_checkTimers();
}

void WasmSequencer ::cmdResponseIn_handler(FwIndexType portNum,
                                           FwOpcodeType opCode,
                                           U32 cmdSeq,
                                           const Fw::CmdResponse& response) {
    // The CmdDisp echoes back the context we sent, not a real cmdSeq. We packed
    // our cmdUid into that context (see makeCmdUid); rename for clarity.
    const U32 cmdUid = cmdSeq;
    const U16 sequenceIndex = static_cast<U16>((cmdUid & 0xFFFF0000) >> 16);
    const U16 cmdIndex = static_cast<U16>(cmdUid & 0xFFFF);
    const U16 currentSequenceIndex = static_cast<U16>(this->m_sequencesStarted & 0xFFFF);
    const U16 currentCmdIndex = static_cast<U16>(this->m_tlm.commandsDispatched & 0xFFFF);

    // If the response is from a previous execution window, treat it as a nominal
    // late reply (e.g. a command that returned after a CANCEL) and just report it
    // without failing the current sequence.
    if (sequenceIndex != currentSequenceIndex) {
        this->log_WARNING_LO_CmdResponseFromOldSequence(opCode, response, sequenceIndex, currentSequenceIndex);
        return;
    }

    // From here on the response claims to be from the current sequence, so any
    // inconsistency is a genuine error that should fail the sequence.
    if (this->interpreter_getState() !=
            WasmSequencer_InterpreterStateMachine_State::RUNNING_AWAITING_RESPONSE_WAITING ||
        this->m_pendingHostFunction.kind != WasmSequencer_HostFunction::COMMAND) {
        this->interpreter_sendSignal_hostResponseUnexpected(WasmSequencer_HostFunction::COMMAND);
        return;
    }

    // Awaiting a command response, but was it for this exact dispatch instance, or
    // an earlier one in this sequence with the same opcode?
    if (cmdIndex != currentCmdIndex) {
        this->log_WARNING_HI_WrongCmdResponseIndex(opCode, response, cmdIndex, currentCmdIndex);
        this->interpreter_sendSignal_hostResponseUnexpected(WasmSequencer_HostFunction::COMMAND);
        return;
    }

    this->m_pendingHostFunction.clear();

    // Track commands that came back with a non-OK response.
    if (response != Fw::CmdResponse::OK) {
        this->m_tlm.commandsFailed++;
    }

    this->interpreter_sendSignal_hostResumeI32(static_cast<I32>(response.e));
}

void WasmSequencer ::writeTelemetry_handler(FwIndexType portNum, U32 context) {
    auto now = this->getTime();

    this->tlmWrite_ControllerState(this->controller_getState(), now);
    this->tlmWrite_InterpreterState(this->interpreter_getState(), now);
    this->tlmWrite_SequencesSucceeded(this->m_tlm.sequencesSucceeded, now);
    this->tlmWrite_SequencesFailed(this->m_tlm.sequencesFailed, now);
    this->tlmWrite_SequencesCancelled(this->m_tlm.sequencesCancelled, now);
    this->tlmWrite_CommandsDispatched(this->m_tlm.commandsDispatched, now);
    this->tlmWrite_CommandsFailed(this->m_tlm.commandsFailed, now);
    this->tlmWrite_LastTrapReason(this->m_exit.lastTrapReason, now);
    this->tlmWrite_SeqName(this->m_tlm.sequenceName, now);
}

void WasmSequencer ::seqRunIn_handler(FwIndexType portNum, const Fw::StringBase& filename, const Svc::SeqArgs& args) {
    Fw::String runModuleName = "";

    this->controller_sendSignal_run(Svc::WasmSequencer_LoadRequest(
        filename, runModuleName, args,
        Svc::WasmSequencer_RequestContext(WasmSequencer_SignalSource::PORT_RUN, WasmSequencer_CommandRequest(0, 0),
                                          BlockState::NO_BLOCK,
                                          /* moduleIdx */ 0  // placeholder, gets filled in after load
                                          )));
}

void WasmSequencer ::seqCancelIn_handler(FwIndexType portNum) {
    this->controller_sendSignal_cancel();
    this->interpreter_sendSignal_cancel();
}

// ----------------------------------------------------------------------
// Handler implementations for serial input ports
// ----------------------------------------------------------------------

void WasmSequencer ::serialIn_handler(FwIndexType portNum, Fw::LinearBufferBase& buffer) {
    FW_ASSERT(portNum < NUM_SERIALIN_INPUT_PORTS, portNum, NUM_SERIALIN_INPUT_PORTS);
    Os::ScopeLock scopeLock(this->m_serialInMutex);
    auto& queue = this->m_serialInQueue[portNum];
    auto fullFullBehavior = this->m_serialInFullBehavior[portNum];

    // Each message is framed on the queue as [U32 length][payload]
    const FwSizeType headerSize = sizeof(U32);
    const FwSizeType payloadSize = buffer.getSize();
    const FwSizeType capacity = queue.get_capacity();

    // Make sure the queue is sized to hold this framed message at all. This does
    // not check free space, only that the queue's capacity is large enough.
    if (headerSize + payloadSize > capacity) {
        this->log_WARNING_HI_SerialInFrameTooLarge(static_cast<U32>(portNum), static_cast<U32>(payloadSize),
                                                   static_cast<U32>(capacity - headerSize));
        return;
    }

    // Total framed size; <= capacity by the assertions above, so it cannot overflow.
    const FwSizeType frameSize = headerSize + payloadSize;

    // Check if we _can_ push the data to the queue
    if (frameSize > queue.get_free_size()) {
        // The queue is full and cannot push this data
        switch (fullFullBehavior) {
            case SerialInQueueFullBehavior::DROP_OLDEST:
                // Drop oldest messages until this one fits.
                while (frameSize > queue.get_free_size()) {
                    U32 nextMsgSize;
                    auto status = queue.peek(nextMsgSize);
                    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, portNum, status);

                    const FwSizeType allocated = queue.get_allocated_size();
                    FW_ASSERT(allocated >= headerSize, portNum, static_cast<FwAssertArgType>(allocated));
                    FW_ASSERT(static_cast<FwSizeType>(nextMsgSize) <= allocated - headerSize, portNum,
                              static_cast<FwAssertArgType>(nextMsgSize), static_cast<FwAssertArgType>(allocated));

                    status = queue.rotate(headerSize + nextMsgSize);

                    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, portNum, status);
                }

                // Now it fits, fall through to push the data to the queue
                break;
            case SerialInQueueFullBehavior::DROP_NEWEST:
                // Drop this message
                return;

            case SerialInQueueFullBehavior::ASSERT:
                FW_ASSERT(false, portNum, static_cast<FwAssertArgType>(frameSize),
                          static_cast<FwAssertArgType>(queue.get_free_size()), static_cast<FwAssertArgType>(capacity));
                break;
        }
    }

    // The message should be able to be put into the queue.
    // Enqueue it as a raw [U32 size][payload] frame. We use the raw (const U8*, size)
    Fw::LinearBufferTemplate<sizeof(U32)> sizeSer;
    auto status = sizeSer.serializeFrom(static_cast<U32>(payloadSize));
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    status = queue.serialize(sizeSer.getBuffAddr(), headerSize);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    status = queue.serialize(buffer.getBuffAddr(), payloadSize);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    // Wake up any blocking recv calls
    this->interpreter_sendSignal_serialInMessage(portNum);
}

// ----------------------------------------------------------------------
// Handler implementations for commands
// ----------------------------------------------------------------------

void WasmSequencer ::RUN_cmdHandler(FwOpcodeType opCode,
                                    U32 cmdSeq,
                                    const Fw::CmdStringArg& fileName,
                                    const Svc::BlockState& block,
                                    const SeqArgs& seqArgs) {
    Fw::String runModuleName = "";
    this->controller_sendSignal_run(Svc::WasmSequencer_LoadRequest(
        fileName, runModuleName, seqArgs,
        Svc::WasmSequencer_RequestContext(WasmSequencer_SignalSource::COMMAND_RUN,
                                          WasmSequencer_CommandRequest(opCode, cmdSeq), block,
                                          /* moduleIdx */ 0  // placeholder, gets filled in after load
                                          )));
}

void WasmSequencer ::WAIT_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    switch (this->controller_getState()) {
        case WasmSequencer_ControllerStateMachine_State::IDLE:
        case WasmSequencer_ControllerStateMachine_State::READY:
            // Nothing is executing, respond immediately
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
            break;
        default: {
            const auto status = this->m_waiting.enqueue(WaitingCmd(opCode, cmdSeq));
            if (status != Fw::Success::SUCCESS) {
                this->log_WARNING_HI_TooManyBlockingCommands();
                this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
            }
        }
    }
}

void WasmSequencer ::LOAD_cmdHandler(FwOpcodeType opCode,
                                     U32 cmdSeq,
                                     const Fw::CmdStringArg& fileName,
                                     const Fw::CmdStringArg& name) {
    this->controller_sendSignal_load(Svc::WasmSequencer_LoadRequest(
        fileName, name, Svc::SeqArgs(),
        Svc::WasmSequencer_RequestContext(WasmSequencer_SignalSource::COMMAND_LOAD,
                                          WasmSequencer_CommandRequest(opCode, cmdSeq), Svc::BlockState::NO_BLOCK,
                                          /* moduleIdx */ 0  // placeholder, gets filled in after load
                                          )));
}

void WasmSequencer ::INVOKE_cmdHandler(FwOpcodeType opCode,
                                       U32 cmdSeq,
                                       const Fw::CmdStringArg& module,
                                       const Svc::BlockState& block,
                                       const Svc::SeqArgs& seqArgs) {
    this->controller_sendSignal_invoke(Svc::WasmSequencer_InvokeRequest(
        module, seqArgs,
        Svc::WasmSequencer_RequestContext(WasmSequencer_SignalSource::COMMAND_INVOKE,
                                          WasmSequencer_CommandRequest(opCode, cmdSeq), block,
                                          /* moduleIdx */ 0  // placeholder, gets filled in after invoke
                                          )));
}

void WasmSequencer ::CANCEL_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    this->controller_sendSignal_cancel();
    this->interpreter_sendSignal_cmdCancel(WasmSequencer_CommandRequest(opCode, cmdSeq));
}

void WasmSequencer ::PAUSE_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    if (this->interpreter_getState() == WasmSequencer_InterpreterStateMachine_State::IDLE) {
        this->log_WARNING_LO_SequenceNotRunning();
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        return;
    }

    this->m_pendingPause = true;
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void WasmSequencer ::CONTINUE_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    switch (this->interpreter_getState()) {
        case WasmSequencer_InterpreterStateMachine_State::RUNNING_AWAITING_RESPONSE_SLEEPING:
        case WasmSequencer_InterpreterStateMachine_State::RUNNING_AWAITING_RESPONSE_WAITING:
        case WasmSequencer_InterpreterStateMachine_State::RUNNING_SPINNING:
            // Already running
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
            break;
        case WasmSequencer_InterpreterStateMachine_State::RUNNING_PAUSED:
            this->interpreter_sendSignal_cmd_CONTINUE();
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
            break;
        case WasmSequencer_InterpreterStateMachine_State::IDLE:
            this->log_WARNING_LO_SequenceNotRunning();
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
            break;
        default:
            FW_ASSERT(false, this->interpreter_getState());
    }
}

void WasmSequencer ::GLOBAL_SET_I32_cmdHandler(FwOpcodeType opCode,
                                               U32 cmdSeq,
                                               const Fw::CmdStringArg& moduleName,
                                               const Fw::CmdStringArg& name,
                                               I32 value) {
    spacewasm_value_t s_value;
    s_value.tag = SPACEWASM_I32;
    s_value.u.i32_ = value;

    auto status = this->setGlobal(moduleName, name, s_value);
    if (status == SPACEWASM_OK) {
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    } else {
        this->log_WARNING_LO_GlobalSetFailed(moduleName, name, status);
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
    }
}

void WasmSequencer ::GLOBAL_SET_I64_cmdHandler(FwOpcodeType opCode,
                                               U32 cmdSeq,
                                               const Fw::CmdStringArg& moduleName,
                                               const Fw::CmdStringArg& name,
                                               I64 value) {
    spacewasm_value_t s_value;
    s_value.tag = SPACEWASM_I64;
    s_value.u.i64_ = value;

    auto status = this->setGlobal(moduleName, name, s_value);
    if (status == SPACEWASM_OK) {
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    } else {
        this->log_WARNING_LO_GlobalSetFailed(moduleName, name, status);
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
    }
}

void WasmSequencer ::GLOBAL_SET_F32_cmdHandler(FwOpcodeType opCode,
                                               U32 cmdSeq,
                                               const Fw::CmdStringArg& moduleName,
                                               const Fw::CmdStringArg& name,
                                               F32 value) {
    spacewasm_value_t s_value;
    s_value.tag = SPACEWASM_F32;
    s_value.u.f32_ = value;

    auto status = this->setGlobal(moduleName, name, s_value);
    if (status == SPACEWASM_OK) {
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    } else {
        this->log_WARNING_LO_GlobalSetFailed(moduleName, name, status);
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
    }
}

void WasmSequencer ::GLOBAL_SET_F64_cmdHandler(FwOpcodeType opCode,
                                               U32 cmdSeq,
                                               const Fw::CmdStringArg& moduleName,
                                               const Fw::CmdStringArg& name,
                                               F64 value) {
    spacewasm_value_t g_value;
    g_value.tag = SPACEWASM_F64;
    g_value.u.f64_ = value;

    auto status = this->setGlobal(moduleName, name, g_value);
    if (status == SPACEWASM_OK) {
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    } else {
        this->log_WARNING_LO_GlobalSetFailed(moduleName, name, status);
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
    }
}

void WasmSequencer ::GLOBAL_GET_cmdHandler(FwOpcodeType opCode,
                                           U32 cmdSeq,
                                           const Fw::CmdStringArg& moduleName,
                                           const Fw::CmdStringArg& name) {
    spacewasm_value_t g_value;
    auto status = this->getGlobal(moduleName, name, g_value);
    if (status == SPACEWASM_OK) {
        switch (g_value.tag) {
            case SPACEWASM_I32:
                this->log_ACTIVITY_LO_GlobalValueI32(moduleName, name, g_value.u.i32_);
                break;
            case SPACEWASM_I64:
                this->log_ACTIVITY_LO_GlobalValueI64(moduleName, name, g_value.u.i64_);
                break;
            case SPACEWASM_F32:
                this->log_ACTIVITY_LO_GlobalValueF32(moduleName, name, g_value.u.f32_);
                break;
            case SPACEWASM_F64:
                this->log_ACTIVITY_LO_GlobalValueF64(moduleName, name, g_value.u.f64_);
                break;
            default:
                FW_ASSERT(false, g_value.tag);
        }

        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    } else {
        this->log_WARNING_LO_GlobalGetFailed(moduleName, name, status);
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
    }
}

void WasmSequencer ::takeAllocatorLock() {
    getGlobalAllocatorLock()->lock();

    auto status = spacewasm_fprime_acquire_global_allocator(this);
    FW_ASSERT(status == SPACEWASM_OK, status);
}

void WasmSequencer ::releaseAllocatorLock() {
    auto status = spacewasm_fprime_release_global_allocator(this);
    FW_ASSERT(status == SPACEWASM_OK, status);

    getGlobalAllocatorLock()->unlock();
}

}  // namespace Svc
