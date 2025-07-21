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

    void add_WAIT_REL(U32 seconds, U32 uSeconds);
    void add_WAIT_REL(FpySequencer_WaitRelDirective dir);
    void add_WAIT_ABS(Fw::Time wakeupTime);
    void add_WAIT_ABS(FpySequencer_WaitAbsDirective dir);
    void add_GOTO(U32 stmtIdx);
    void add_GOTO(FpySequencer_GotoDirective dir);
    void add_SET_SER_REG(U8 serRegIdx, Fw::StatementArgBuffer value);
    void add_SET_SER_REG(FpySequencer_SetSerRegDirective dir);
    void add_IF(U8 serRegIdx, U32 gotoIfFalse);
    void add_IF(FpySequencer_IfDirective dir);
    void add_NO_OP();
    void add_GET_TLM(U8 valueDestSerReg, U8 timeDestSerReg, FwChanIdType id);
    void add_GET_TLM(FpySequencer_GetTlmDirective dir);
    void add_GET_PRM(U8 serRegIdx, FwPrmIdType id);
    void add_GET_PRM(FpySequencer_GetPrmDirective dir);
    void add_CMD(FwOpcodeType opcode);
    void add_CMD(FpySequencer_CmdDirective dir);
    void add_DESER_SER_REG(U8 srcSerRegIdx, FwSizeType srcOffset, U8 destReg, U8 deserSize);
    void add_DESER_SER_REG(FpySequencer_DeserSerRegDirective dir);
    void add_SET_REG(U8 dest, I64 value);
    void add_SET_REG(FpySequencer_SetRegDirective dir);
    void add_BINARY_REG_OP(U8 lhs, U8 rhs, U8 res, Fpy::DirectiveId op);
    void add_BINARY_REG_OP(FpySequencer_BinaryRegOpDirective dir);
    void add_UNARY_REG_OP(U8 src, U8 res, Fpy::DirectiveId op);
    void add_UNARY_REG_OP(FpySequencer_UnaryRegOpDirective dir);
    void add_EXIT(bool success);
    void add_EXIT(FpySequencer_ExitDirective dir);
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
    Signal tester_goto_directiveHandler(const Svc::FpySequencer_GotoDirective &directive, DirectiveError& err);
    Signal tester_setSerReg_directiveHandler(const FpySequencer_SetSerRegDirective& directive, DirectiveError& err);
    Signal tester_if_directiveHandler(const FpySequencer_IfDirective& directive, DirectiveError& err);
    Signal tester_getPrm_directiveHandler(const FpySequencer_GetPrmDirective& directive, DirectiveError& err);
    Signal tester_getTlm_directiveHandler(const FpySequencer_GetTlmDirective& directive, DirectiveError& err);
    Signal tester_exit_directiveHandler(const FpySequencer_ExitDirective& directive, DirectiveError& err);
    Signal tester_cmd_directiveHandler(const FpySequencer_CmdDirective& directive, DirectiveError& err);
    Signal tester_deserSerReg_directiveHandler(const FpySequencer_DeserSerRegDirective& directive, DirectiveError& err);
    Signal tester_unaryRegOp_directiveHandler(const FpySequencer_UnaryRegOpDirective& directive, DirectiveError& err);
    Signal tester_binaryRegOp_directiveHandler(const FpySequencer_BinaryRegOpDirective& directive, DirectiveError& err);
    Signal tester_setReg_directiveHandler(const FpySequencer_SetRegDirective& directive, DirectiveError& err);
    I64 tester_binaryRegOp_or(I64 lhs, I64 rhs);
    I64 tester_binaryRegOp_and(I64 lhs, I64 rhs);
    I64 tester_binaryRegOp_ieq(I64 lhs, I64 rhs);
    I64 tester_binaryRegOp_ine(I64 lhs, I64 rhs);
    I64 tester_binaryRegOp_ult(I64 lhs, I64 rhs);
    I64 tester_binaryRegOp_ule(I64 lhs, I64 rhs);
    I64 tester_binaryRegOp_ugt(I64 lhs, I64 rhs);
    I64 tester_binaryRegOp_uge(I64 lhs, I64 rhs);
    I64 tester_binaryRegOp_slt(I64 lhs, I64 rhs);
    I64 tester_binaryRegOp_sle(I64 lhs, I64 rhs);
    I64 tester_binaryRegOp_sgt(I64 lhs, I64 rhs);
    I64 tester_binaryRegOp_sge(I64 lhs, I64 rhs);
    I64 tester_binaryRegOp_feq(I64 lhs, I64 rhs);
    I64 tester_binaryRegOp_fne(I64 lhs, I64 rhs);
    I64 tester_binaryRegOp_flt(I64 lhs, I64 rhs);
    I64 tester_binaryRegOp_fle(I64 lhs, I64 rhs);
    I64 tester_binaryRegOp_fgt(I64 lhs, I64 rhs);
    I64 tester_binaryRegOp_fge(I64 lhs, I64 rhs);
    I64 tester_unaryRegOp_not(I64 src);
    I64 tester_unaryRegOp_fpext(I64 src);
    I64 tester_unaryRegOp_fptrunc(I64 src);
    I64 tester_unaryRegOp_fptoui(I64 src);
    I64 tester_unaryRegOp_uitofp(I64 src);
    I64 tester_unaryRegOp_fptosi(I64 src);
    I64 tester_unaryRegOp_sitofp(I64 src);
    FpySequencer::Runtime* tester_get_m_runtime_ptr();
    Fw::ExternalSerializeBuffer* tester_get_m_sequenceBuffer_ptr();
    void tester_set_m_sequencesStarted(U64 val);
    void tester_set_m_statementsDispatched(U64 val);
    U64 tester_get_m_sequencesStarted();
    U64 tester_get_m_statementsDispatched();
    Fw::Success tester_deserializeDirective(const Fpy::Statement& stmt, Svc::FpySequencer::DirectiveUnion& deserializedDirective);
    Fpy::Sequence* tester_get_m_sequenceObj_ptr();
    Svc::Signal tester_dispatchStatement();
    Fw::Success tester_validate();
    Fw::String tester_get_m_sequenceFilePath();
    void tester_set_m_sequenceFilePath(Fw::String str);
    Fw::Success tester_readBytes(Os::File& file, FwSizeType readLen, FpySequencer_FileReadStage readStage, bool updateCrc = true);
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

  public:
    // ----------------------------------------------------------------------
    // Static methods for accessing protected opcodes
    // ----------------------------------------------------------------------

    //! Get the OPCODE_RUN value
    static FwOpcodeType get_OPCODE_RUN() {
        return FpySequencerComponentBase::OPCODE_RUN;
    }

    //! Get the OPCODE_VALIDATE value
    static FwOpcodeType get_OPCODE_VALIDATE() {
        return FpySequencerComponentBase::OPCODE_VALIDATE;
    }

    //! Get the OPCODE_RUN_VALIDATED value
    static FwOpcodeType get_OPCODE_RUN_VALIDATED() {
        return FpySequencerComponentBase::OPCODE_RUN_VALIDATED;
    }

    //! Get the OPCODE_CANCEL value
    static FwOpcodeType get_OPCODE_CANCEL() {
        return FpySequencerComponentBase::OPCODE_CANCEL;
    }

    //! Get the OPCODE_DEBUG_CLEAR_BREAKPOINT value
    static FwOpcodeType get_OPCODE_DEBUG_CLEAR_BREAKPOINT() {
        return FpySequencerComponentBase::OPCODE_DEBUG_CLEAR_BREAKPOINT;
    }

    //! Get the OPCODE_DEBUG_SET_BREAKPOINT value
    static FwOpcodeType get_OPCODE_DEBUG_SET_BREAKPOINT() {
        return FpySequencerComponentBase::OPCODE_DEBUG_SET_BREAKPOINT;
    }

    //! Get the OPCODE_DEBUG_BREAK value
    static FwOpcodeType get_OPCODE_DEBUG_BREAK() {
        return FpySequencerComponentBase::OPCODE_DEBUG_BREAK;
    }

    //! Get the OPCODE_DEBUG_CONTINUE value
    static FwOpcodeType get_OPCODE_DEBUG_CONTINUE() {
        return FpySequencerComponentBase::OPCODE_DEBUG_CONTINUE;
    }


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
    static void setState(FpySequencer_SequencerStateMachineStateMachineBase& sm, State s){
        sm.m_state = s;
    }

};

}  // namespace Svc

#endif
