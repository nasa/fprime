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
    void addStmt(const Fpy::Statement& stmt);
    void addCmd(FwOpcodeType opcode);
    void addDirective(Fpy::DirectiveId id, Fw::StatementArgBuffer& buf);

    void add_WAIT_REL(Fw::TimeInterval duration);
    void add_WAIT_REL(FpySequencer_WaitRelDirective dir);
    void add_WAIT_ABS(Fw::Time wakeupTime);
    void add_WAIT_ABS(FpySequencer_WaitAbsDirective dir);
    void add_GOTO(U32 stmtIdx);
    void add_GOTO(FpySequencer_GotoDirective dir);
    void add_SET_LVAR(U8 lvarIdx, Fw::StatementArgBuffer value);
    void add_SET_LVAR(FpySequencer_SetLocalVarDirective dir);
    void add_IF(U8 lvarIdx, U32 gotoIfFalse);
    void add_IF(FpySequencer_IfDirective dir);
    void add_NO_OP();
    void add_GET_TLM(U8 valueDestLvar, U8 timeDestLvar, FwChanIdType id);
    void add_GET_TLM(FpySequencer_GetTlmDirective dir);
    void add_GET_PRM(U8 lvarIdx, FwPrmIdType id);
    void add_GET_PRM(FpySequencer_GetPrmDirective dir);
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
};

}  // namespace Svc

#endif
