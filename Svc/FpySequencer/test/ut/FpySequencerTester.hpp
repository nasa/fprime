// ======================================================================
// \title  FpySequencer/test/ut/Tester.hpp
// \author zimri.leisher
// \brief  hpp file for FpySequencer test harness implementation class
// ======================================================================

#ifndef FPYSEQUENCER_TESTER_HPP
#define FPYSEQUENCER_TESTER_HPP

#include "FpySequencerGTestBase.hpp"
#include "Svc/FpySequencer/FpySequencer.hpp"

namespace Svc {

class FpySequencerTester : public FpySequencerGTestBase, public ::testing::Test {
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

  public:
    // Maximum size of histories storing events, telemetry, and port outputs
    static const FwSizeType MAX_HISTORY_SIZE = 10;
    // Instance ID supplied to the component instance under test
    static const FwSizeType TEST_INSTANCE_ID = 0;
    // Queue depth supplied to component instance under test
    static const FwSizeType TEST_INSTANCE_QUEUE_DEPTH = 10;

    //! Construct object FpySequencerTester
    //!
    FpySequencerTester();

    //! Destroy object FpySequencerTester
    //!
    ~FpySequencerTester();

  private:
    // ----------------------------------------------------------------------
    // Helper methods
    // ----------------------------------------------------------------------

    //! Connect ports
    //!
    void connectPorts();

    //! Initialize components
    //!
    void initComponents();

  protected:
    // ----------------------------------------------------------------------
    // Variables
    // ----------------------------------------------------------------------

    //! The component under test
    //!
    FpySequencer cmp;
    FpySequencer& component;  // for compatibility

    // dispatches events from the queue until the component reaches the given state
    void dispatchUntilState(State state, U32 bound = 100);
    void resetRuntime();

    // a sequence that you can build with the following functions
    Fpy::Sequence seq;
    U8 internalSeqBuf[Fpy::Sequence::SERIALIZED_SIZE] = {0};

    FwChanIdType nextTlmId;
    Fw::Time nextTlmTime;
    Fw::TlmBuffer nextTlmValue;

    FwPrmIdType nextPrmId;
    Fw::ParamBuffer nextPrmValue;

    // clears the sequence we're currently building
    void clearSeq();
    void allocMem(FwSizeType bytes = Fpy::Sequence::SERIALIZED_SIZE);
    // writes the sequence we're building to a file with the given name
    // writes up to maxBytes bytes
    void writeToFile(const char* name, FwSizeType maxBytes = Fpy::Sequence::SERIALIZED_SIZE);
    void removeFile(const char* name);
    void addDirective(Fpy::DirectiveId id, Fw::StatementArgBuffer& buf);

    void add_WAIT_REL();
    void add_WAIT_REL(FpySequencer_WaitRelDirective dir);
    void add_WAIT_ABS();
    void add_WAIT_ABS(FpySequencer_WaitAbsDirective dir);
    void add_GOTO(U32 stmtIdx);
    void add_GOTO(FpySequencer_GotoDirective dir);
    void add_IF(U32 gotoIfFalse);
    void add_IF(FpySequencer_IfDirective dir);
    void add_NO_OP();
    void add_STORE_TLM_VAL(FwChanIdType id, U16 lvarOffset);
    void add_STORE_TLM_VAL(FpySequencer_StoreTlmValDirective dir);
    void add_STORE_PRM(FwPrmIdType id, U16 lvarOffset);
    void add_STORE_PRM(FpySequencer_StorePrmDirective dir);
    void add_CONST_CMD(FwOpcodeType opcode);
    void add_CONST_CMD(FpySequencer_ConstCmdDirective dir);
    void add_STACK_OP(Fpy::DirectiveId op);
    void add_EXIT();
    void add_ALLOCATE(U16 size);
    void add_ALLOCATE(FpySequencer_AllocateDirective dir);
    void add_STORE(U16 lvarOffset, U16 size);
    void add_STORE(FpySequencer_StoreDirective dir);
    void add_LOAD(U16 lvarOffset, U16 size);
    void add_LOAD(FpySequencer_LoadDirective dir);
    void add_DISCARD(U16 size);
    void add_DISCARD(FpySequencer_DiscardDirective dir);
    void add_STACK_CMD(U16 size);
    void add_STACK_CMD(FpySequencer_StackCmdDirective dir);
    void add_MEMCMP(U16 size);
    void add_MEMCMP(FpySequencer_MemCmpDirective dir);
    template <typename T>
    void add_PUSH_VAL(T val);
    //! Handle a text event
    void textLogIn(FwEventIdType id,                //!< The event ID
                   const Fw::Time& timeTag,         //!< The time
                   const Fw::LogSeverity severity,  //!< The severity
                   const Fw::TextLogString& text    //!< The event string
                   ) override;

    void writeAndRun();

    //! Default handler implementation for from_getTlmChan
    Fw::TlmValid from_getTlmChan_handler(FwIndexType portNum,  //!< The port number
                                         FwChanIdType id,      //!< Telemetry Channel ID
                                         Fw::Time& timeTag,    //!< Time Tag
                                         Fw::TlmBuffer& val    //!< Buffer containing serialized telemetry value.
                                                               //!< Size set to 0 if channel not found.
                                         ) override;

    //! Default handler implementation for from_getParam
    Fw::ParamValid from_getParam_handler(FwIndexType portNum,  //!< The port number
                                         FwPrmIdType id,       //!< Parameter ID
                                         Fw::ParamBuffer& val  //!< Buffer containing serialized parameter value.
                                                               //!< Unmodified if param not found.
                                         ) override;

    // Access to private and protected FpySequencer methods and members for UTs
    Signal tester_noOp_directiveHandler(const FpySequencer_NoOpDirective& directive, DirectiveError& err);
    Signal tester_waitRel_directiveHandler(const FpySequencer_WaitRelDirective& directive, DirectiveError& err);
    Signal tester_waitAbs_directiveHandler(const FpySequencer_WaitAbsDirective& directive, DirectiveError& err);
    Signal tester_goto_directiveHandler(const Svc::FpySequencer_GotoDirective& directive, DirectiveError& err);
    Signal tester_if_directiveHandler(const FpySequencer_IfDirective& directive, DirectiveError& err);
    Signal tester_storePrm_directiveHandler(const FpySequencer_StorePrmDirective& directive, DirectiveError& err);
    Signal tester_storeTlmVal_directiveHandler(const FpySequencer_StoreTlmValDirective& directive, DirectiveError& err);
    Signal tester_exit_directiveHandler(const FpySequencer_ExitDirective& directive, DirectiveError& err);
    Signal tester_constCmd_directiveHandler(const FpySequencer_ConstCmdDirective& directive, DirectiveError& err);
    Signal tester_stackOp_directiveHandler(const FpySequencer_StackOpDirective& directive, DirectiveError& err);
    Signal tester_discard_directiveHandler(const FpySequencer_DiscardDirective& directive, DirectiveError& err);
    Signal tester_stackCmd_directiveHandler(const FpySequencer_StackCmdDirective& directive, DirectiveError& err);
    Signal tester_memCmp_directiveHandler(const FpySequencer_MemCmpDirective& directive, DirectiveError& err);
    DirectiveError tester_op_or();
    DirectiveError tester_op_and();
    DirectiveError tester_op_ieq();
    DirectiveError tester_op_ine();
    DirectiveError tester_op_ult();
    DirectiveError tester_op_ule();
    DirectiveError tester_op_ugt();
    DirectiveError tester_op_uge();
    DirectiveError tester_op_slt();
    DirectiveError tester_op_sle();
    DirectiveError tester_op_sgt();
    DirectiveError tester_op_sge();
    DirectiveError tester_op_feq();
    DirectiveError tester_op_fne();
    DirectiveError tester_op_flt();
    DirectiveError tester_op_fle();
    DirectiveError tester_op_fgt();
    DirectiveError tester_op_fge();
    DirectiveError tester_op_not();
    DirectiveError tester_op_fpext();
    DirectiveError tester_op_fptrunc();
    DirectiveError tester_op_fptoui();
    DirectiveError tester_op_fptosi();
    DirectiveError tester_op_sitofp();
    DirectiveError tester_op_uitofp();
    DirectiveError tester_op_iadd();
    DirectiveError tester_op_isub();
    DirectiveError tester_op_imul();
    DirectiveError tester_op_udiv();
    DirectiveError tester_op_sdiv();
    DirectiveError tester_op_umod();
    DirectiveError tester_op_smod();
    DirectiveError tester_op_fadd();
    DirectiveError tester_op_fsub();
    DirectiveError tester_op_fmul();
    DirectiveError tester_op_fdiv();
    DirectiveError tester_op_float_floor_div();
    DirectiveError tester_op_fpow();
    DirectiveError tester_op_flog();
    DirectiveError tester_op_fmod();
    DirectiveError tester_op_siext_8_64();
    DirectiveError tester_op_siext_16_64();
    DirectiveError tester_op_siext_32_64();
    DirectiveError tester_op_ziext_8_64();
    DirectiveError tester_op_ziext_16_64();
    DirectiveError tester_op_ziext_32_64();
    DirectiveError tester_op_itrunc_64_8();
    DirectiveError tester_op_itrunc_64_16();
    DirectiveError tester_op_itrunc_64_32();
    FpySequencer::Runtime* tester_get_m_runtime_ptr();
    Fw::ExternalSerializeBuffer* tester_get_m_sequenceBuffer_ptr();
    void tester_set_m_sequencesStarted(U64 val);
    void tester_set_m_statementsDispatched(U64 val);
    U64 tester_get_m_sequencesStarted();
    U64 tester_get_m_statementsDispatched();
    Fw::Success tester_deserializeDirective(const Fpy::Statement& stmt,
                                            Svc::FpySequencer::DirectiveUnion& deserializedDirective);
    Fpy::Sequence* tester_get_m_sequenceObj_ptr();
    Svc::Signal tester_dispatchStatement();
    Fw::Success tester_validate();
    Fw::String tester_get_m_sequenceFilePath();
    void tester_set_m_sequenceFilePath(Fw::String str);
    Fw::Success tester_readBytes(Os::File& file,
                                 FwSizeType readLen,
                                 FpySequencer_FileReadStage readStage,
                                 bool updateCrc = true);
    Fw::Success tester_readFooter();
    Fw::Success tester_readBody();
    Fw::Success tester_readHeader();
    void tester_set_m_computedCRC(U32 crc);
    Svc::FpySequencer::Debug* tester_get_m_debug_ptr();
    Svc::Signal tester_checkStatementTimeout();
    Svc::Signal tester_checkShouldWake();
    Svc::FpySequencer::Telemetry* tester_get_m_tlm_ptr();
    void tester_doDispatch();
    void tester_setState(Svc::FpySequencer_SequencerStateMachineStateMachineBase::State state);
    Svc::FpySequencer_SequencerStateMachineStateMachineBase::State tester_getState();
    void tester_dispatchDirective(const FpySequencer::DirectiveUnion& directive, const Fpy::DirectiveId& id);
    template <typename T>
    void tester_push(T val);
    template <typename T>
    T tester_pop();

  public:
    // ----------------------------------------------------------------------
    // Static methods for accessing protected opcodes
    // ----------------------------------------------------------------------

    //! Get the OPCODE_RUN value
    static FwOpcodeType get_OPCODE_RUN() { return FpySequencerComponentBase::OPCODE_RUN; }

    //! Get the OPCODE_VALIDATE value
    static FwOpcodeType get_OPCODE_VALIDATE() { return FpySequencerComponentBase::OPCODE_VALIDATE; }

    //! Get the OPCODE_RUN_VALIDATED value
    static FwOpcodeType get_OPCODE_RUN_VALIDATED() { return FpySequencerComponentBase::OPCODE_RUN_VALIDATED; }

    //! Get the OPCODE_CANCEL value
    static FwOpcodeType get_OPCODE_CANCEL() { return FpySequencerComponentBase::OPCODE_CANCEL; }

    //! Get the OPCODE_DEBUG_CLEAR_BREAKPOINT value
    static FwOpcodeType get_OPCODE_DEBUG_CLEAR_BREAKPOINT() {
        return FpySequencerComponentBase::OPCODE_DEBUG_CLEAR_BREAKPOINT;
    }

    //! Get the OPCODE_DEBUG_SET_BREAKPOINT value
    static FwOpcodeType get_OPCODE_DEBUG_SET_BREAKPOINT() {
        return FpySequencerComponentBase::OPCODE_DEBUG_SET_BREAKPOINT;
    }

    //! Get the OPCODE_DEBUG_BREAK value
    static FwOpcodeType get_OPCODE_DEBUG_BREAK() { return FpySequencerComponentBase::OPCODE_DEBUG_BREAK; }

    //! Get the OPCODE_DEBUG_CONTINUE value
    static FwOpcodeType get_OPCODE_DEBUG_CONTINUE() { return FpySequencerComponentBase::OPCODE_DEBUG_CONTINUE; }
};

class FpySequencer_SequencerStateMachineTester {
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

  public:
    //! Construct object FpySequencer_SequencerStateMachineTester
    //!
    FpySequencer_SequencerStateMachineTester();

    //! Destroy object FpySequencer_SequencerStateMachineTester
    //!
    ~FpySequencer_SequencerStateMachineTester();

    // ----------------------------------------------------------------------
    // Test access to private and protected methods and members
    // ----------------------------------------------------------------------
    static void setState(FpySequencer_SequencerStateMachineStateMachineBase& sm, State s) { sm.m_state = s; }
};

}  // namespace Svc

#endif
