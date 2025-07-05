// ======================================================================
// \title  FpySequencer.hpp
// \author zimri.leisher
// \brief  cpp file for FpySequencer test harness implementation class
// ======================================================================

#include "FpySequencerTester.hpp"
#include "Os/FileSystem.hpp"
#include "Svc/FpySequencer/FppConstantsAc.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

FpySequencerTester ::FpySequencerTester()
    : FpySequencerGTestBase("FpySequencerTester", FpySequencerTester::MAX_HISTORY_SIZE),
      cmp("FpySequencer"),
      component(cmp) {
    this->connectPorts();
    this->initComponents();
    clearSeq();
}

FpySequencerTester ::~FpySequencerTester() {}

// dispatches events from the queue until the cmp reaches the given state
void FpySequencerTester::dispatchUntilState(State state, U32 bound) {
    U64 iters = 0;
    while (cmp.sequencer_getState() != state && iters < bound) {
        if (cmp.m_queue.getMessagesAvailable() == 0) {
            break;
        }
        cmp.doDispatch();
        iters++;
    }
    ASSERT_EQ(cmp.sequencer_getState(), state);
}

void FpySequencerTester::allocMem(FwSizeType bytes) {
    if (bytes > sizeof(internalSeqBuf)) {
        bytes = sizeof(internalSeqBuf);
    }
    cmp.m_sequenceBuffer.setExtBuffer(internalSeqBuf, bytes);
}

void FpySequencerTester::clearSeq() {
    seq = Fpy::Sequence();
    seq.getheader().setschemaVersion(Fpy::SCHEMA_VERSION);
}

// writes the sequence we're building to a file with the given name
void FpySequencerTester::writeToFile(const char* name, FwSizeType maxBytes) {
    Os::File outputFile;
    ASSERT_EQ(outputFile.open(name, Os::FileInterface::OPEN_CREATE, Os::FileInterface::OverwriteType::OVERWRITE),
              Os::File::Status::OP_OK);

    // TODO is this okay to declare on stack? this could stack overflow, it's like 500 KB with default configs
    U8 data[Fpy::Sequence::SERIALIZED_SIZE];
    Fw::ExternalSerializeBuffer buf;
    buf.setExtBuffer(data, sizeof(data));

    // first let's calculate the size of the body. do this by just writing the body,
    // then calculating how big that was, then clearing and writing the header, then writing the body again
    for (U32 ii = 0; ii < seq.getheader().getargumentCount(); ii++) {
        ASSERT_EQ(buf.serialize(seq.getargs()[ii]), Fw::SerializeStatus::FW_SERIALIZE_OK);
    }
    for (U32 ii = 0; ii < seq.getheader().getstatementCount(); ii++) {
        ASSERT_EQ(buf.serialize(seq.getstatements()[ii]), Fw::SerializeStatus::FW_SERIALIZE_OK);
    }
    seq.getheader().setbodySize(static_cast<U32>(buf.getBuffLength()));
    buf.resetSer();

    ASSERT_EQ(buf.serialize(seq.getheader()), Fw::SerializeStatus::FW_SERIALIZE_OK);
    for (U32 ii = 0; ii < seq.getheader().getargumentCount(); ii++) {
        ASSERT_EQ(buf.serialize(seq.getargs()[ii]), Fw::SerializeStatus::FW_SERIALIZE_OK);
    }
    for (U32 ii = 0; ii < seq.getheader().getstatementCount(); ii++) {
        ASSERT_EQ(buf.serialize(seq.getstatements()[ii]), Fw::SerializeStatus::FW_SERIALIZE_OK);
    }

    U32 crc = FpySequencer::CRC_INITIAL_VALUE;
    FpySequencer::updateCrc(crc, buf.getBuffAddr(), buf.getBuffLength());

    seq.getfooter().setcrc(~crc);

    ASSERT_EQ(buf.serialize(seq.getfooter()), Fw::SerializeStatus::FW_SERIALIZE_OK);

    FwSizeType intendedWriteSize = buf.getBuffLength();
    if (intendedWriteSize > maxBytes) {
        intendedWriteSize = maxBytes;
    }
    FwSizeType actualWriteSize = intendedWriteSize;
    ASSERT_EQ(outputFile.write(buf.getBuffAddr(), actualWriteSize), Os::File::Status::OP_OK);
    ASSERT_EQ(intendedWriteSize, actualWriteSize);

    outputFile.close();
}

void FpySequencerTester::removeFile(const char* name) {
    Os::FileSystem::removeFile(name);
}

void FpySequencerTester::resetRuntime() {
    // explicitly call dtor
    cmp.m_runtime.~Runtime();
    new (&cmp.m_runtime) FpySequencer::Runtime();
}

void FpySequencerTester::addDirective(Fpy::DirectiveId id, Fw::StatementArgBuffer& buf) {
    Fpy::Statement stmt(id.e, buf);

    // if fails, cannot add a new stmt (out of space)
    FW_ASSERT(seq.getheader().getstatementCount() < std::numeric_limits<U16>::max());

    U16 stateCount = seq.getheader().getstatementCount();
    seq.getstatements()[stateCount] = stmt;
    seq.getheader().setstatementCount(static_cast<U16>(stateCount + 1));
}

void FpySequencerTester::add_WAIT_REL(U32 seconds, U32 uSeconds) {
    add_WAIT_REL(FpySequencer_WaitRelDirective(seconds, uSeconds));
}

void FpySequencerTester::add_WAIT_REL(FpySequencer_WaitRelDirective dir) {
    Fw::StatementArgBuffer buf;
    FW_ASSERT(buf.serialize(dir) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    addDirective(Fpy::DirectiveId::WAIT_REL, buf);
}

void FpySequencerTester::add_WAIT_ABS(Fw::Time wakeupTime) {
    add_WAIT_ABS(FpySequencer_WaitAbsDirective(wakeupTime));
}

void FpySequencerTester::add_WAIT_ABS(FpySequencer_WaitAbsDirective dir) {
    Fw::StatementArgBuffer buf;
    FW_ASSERT(buf.serialize(dir) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    addDirective(Fpy::DirectiveId::WAIT_ABS, buf);
}

void FpySequencerTester::add_GOTO(U32 stmtIdx) {
    add_GOTO(FpySequencer_GotoDirective(stmtIdx));
}

void FpySequencerTester::add_GOTO(FpySequencer_GotoDirective dir) {
    Fw::StatementArgBuffer buf;
    FW_ASSERT(buf.serialize(dir) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    addDirective(Fpy::DirectiveId::GOTO, buf);
}

void FpySequencerTester::add_SET_SER_REG(U8 serRegIdx, Fw::StatementArgBuffer value) {
    add_SET_SER_REG(FpySequencer_SetSerRegDirective(serRegIdx, *value.getBuffAddr(), value.getBuffLength()));
}

void FpySequencerTester::add_SET_SER_REG(FpySequencer_SetSerRegDirective dir) {
    Fw::StatementArgBuffer buf;
    FW_ASSERT(buf.serialize(dir.getindex()) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    FW_ASSERT(buf.serialize(dir.getvalue(), dir.get_valueSize(), Fw::Serialization::OMIT_LENGTH) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    addDirective(Fpy::DirectiveId::SET_SER_REG, buf);
}

void FpySequencerTester::add_IF(U8 serRegIdx, U32 gotoIfFalse) {
    add_IF(FpySequencer_IfDirective(serRegIdx, gotoIfFalse));
}

void FpySequencerTester::add_IF(FpySequencer_IfDirective dir) {
    Fw::StatementArgBuffer buf;
    FW_ASSERT(buf.serialize(dir) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    addDirective(Fpy::DirectiveId::IF, buf);
}

void FpySequencerTester::add_NO_OP() {
    Fw::StatementArgBuffer buf;
    addDirective(Fpy::DirectiveId::NO_OP, buf);
}

void FpySequencerTester::add_GET_TLM(U8 valueDestSerReg, U8 timeDestSerReg, FwChanIdType id) {
    add_GET_TLM(FpySequencer_GetTlmDirective(valueDestSerReg, timeDestSerReg, id));
}

void FpySequencerTester::add_GET_TLM(FpySequencer_GetTlmDirective dir) {
    Fw::StatementArgBuffer buf;
    FW_ASSERT(buf.serialize(dir) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    addDirective(Fpy::DirectiveId::GET_TLM, buf);
}

void FpySequencerTester::add_GET_PRM(U8 serRegIdx, FwPrmIdType id) {
    add_GET_PRM(FpySequencer_GetPrmDirective(serRegIdx, id));
}

void FpySequencerTester::add_GET_PRM(FpySequencer_GetPrmDirective dir) {
    Fw::StatementArgBuffer buf;
    FW_ASSERT(buf.serialize(dir) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    addDirective(Fpy::DirectiveId::GET_PRM, buf);
}

void FpySequencerTester::add_CMD(FwOpcodeType opcode) {
    add_CMD(FpySequencer_CmdDirective(opcode, 0, 0));
}

void FpySequencerTester::add_CMD(FpySequencer_CmdDirective dir) {
    Fw::StatementArgBuffer buf;
    FW_ASSERT(buf.serialize(dir.getopCode()) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    FW_ASSERT(buf.serialize(dir.getargBuf(), dir.get_argBufSize(), Fw::Serialization::OMIT_LENGTH) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    addDirective(Fpy::DirectiveId::CMD, buf);
}

void FpySequencerTester::add_DESER_SER_REG(U8 srcSerRegIdx, FwSizeType srcOffset, U8 destReg, U8 deserSize) {
    add_DESER_SER_REG(FpySequencer_DeserSerRegDirective(srcSerRegIdx, srcOffset, destReg, deserSize));
}

void FpySequencerTester::add_DESER_SER_REG(FpySequencer_DeserSerRegDirective dir) {
    Fw::StatementArgBuffer buf;
    FW_ASSERT(buf.serialize(dir.getsrcSerRegIdx()) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    FW_ASSERT(buf.serialize(dir.getsrcOffset()) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    FW_ASSERT(buf.serialize(dir.getdestReg()) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    Fpy::DirectiveId id;
    if (dir.get_deserSize() == 1) {
        id = Fpy::DirectiveId::DESER_SER_REG_1;
    } else if (dir.get_deserSize() == 2) {
        id = Fpy::DirectiveId::DESER_SER_REG_2;
    } else if (dir.get_deserSize() == 4) {
        id = Fpy::DirectiveId::DESER_SER_REG_4;
    } else if (dir.get_deserSize() == 8) {
        id = Fpy::DirectiveId::DESER_SER_REG_8;
    } else {
        FW_ASSERT(0, static_cast<FwAssertArgType>(dir.get_deserSize()));
    }

    addDirective(id, buf);
}

void FpySequencerTester::add_SET_REG(U8 dest, I64 value) {
    add_SET_REG(FpySequencer_SetRegDirective(dest, value));
}

void FpySequencerTester::add_SET_REG(FpySequencer_SetRegDirective dir) {
    Fw::StatementArgBuffer buf;
    FW_ASSERT(buf.serialize(dir) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    addDirective(Fpy::DirectiveId::SET_REG, buf);
}

void FpySequencerTester::add_BINARY_REG_OP(U8 lhs, U8 rhs, U8 res, Fpy::DirectiveId op) {
    add_BINARY_REG_OP(FpySequencer_BinaryRegOpDirective(lhs, rhs, res, op));
}

void FpySequencerTester::add_BINARY_REG_OP(FpySequencer_BinaryRegOpDirective dir) {
    Fw::StatementArgBuffer buf;
    FW_ASSERT(buf.serialize(dir.getlhs()) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    FW_ASSERT(buf.serialize(dir.getrhs()) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    FW_ASSERT(buf.serialize(dir.getres()) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    addDirective(dir.get_op(), buf);
}

void FpySequencerTester::add_UNARY_REG_OP(U8 src, U8 res, Fpy::DirectiveId op) {
    add_UNARY_REG_OP(FpySequencer_UnaryRegOpDirective(src, res, op));
}

void FpySequencerTester::add_UNARY_REG_OP(FpySequencer_UnaryRegOpDirective dir) {
    Fw::StatementArgBuffer buf;
    FW_ASSERT(buf.serialize(dir.getsrc()) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    FW_ASSERT(buf.serialize(dir.getres()) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    addDirective(dir.get_op(), buf);
}
void FpySequencerTester::add_EXIT(bool success) {
    add_EXIT(FpySequencer_ExitDirective(success));
}

void FpySequencerTester::add_EXIT(FpySequencer_ExitDirective dir) {
    Fw::StatementArgBuffer buf;
    FW_ASSERT(buf.serialize(dir) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    addDirective(Fpy::DirectiveId::EXIT, buf);
}

//! Handle a text event
void FpySequencerTester::textLogIn(FwEventIdType id,                //!< The event ID
                                   const Fw::Time& timeTag,         //!< The time
                                   const Fw::LogSeverity severity,  //!< The severity
                                   const Fw::TextLogString& text    //!< The event string
) {
    // printf("%s\n", text.toChar());
}

void FpySequencerTester::writeAndRun() {
    removeFile("test.bin");
    writeToFile("test.bin");
    sendCmd_RUN(0, 0, Fw::String("test.bin"), FpySequencer_BlockState::BLOCK);
    // dispatch cmd
    cmp.doDispatch();
    // dispatch sm sig
    cmp.doDispatch();
}

//! Default handler implementation for from_getTlmChan
Fw::TlmValid FpySequencerTester::from_getTlmChan_handler(
    FwIndexType portNum,  //!< The port number
    FwChanIdType id,      //!< Telemetry Channel ID
    Fw::Time& timeTag,    //!< Time Tag
    Fw::TlmBuffer& val    //!< Buffer containing serialized telemetry value.
                          //!< Size set to 0 if channel not found.
) {
    this->pushFromPortEntry_getTlmChan(id, timeTag, val);
    if (id != nextTlmId) {
        val.setBuffLen(0);
        return Fw::TlmValid::INVALID;
    }
    val = nextTlmValue;
    timeTag = nextTlmTime;
    return Fw::TlmValid::VALID;
}

//! Default handler implementation for from_getParam
Fw::ParamValid FpySequencerTester::from_getParam_handler(
    FwIndexType portNum,  //!< The port number
    FwPrmIdType id,       //!< Parameter ID
    Fw::ParamBuffer& val  //!< Buffer containing serialized parameter value.
                          //!< Unmodified if param not found.
) {
    this->pushFromPortEntry_getParam(id, val);
    if (id != nextPrmId) {
        return Fw::ParamValid::INVALID;
    }
    val = nextPrmValue;
    return Fw::ParamValid::VALID;
}

// Access to private and protected FpySequencer methods and members for UTs

// Call cmp methods
Signal FpySequencerTester::tester_noOp_directiveHandler(const FpySequencer_NoOpDirective& directive,
                                                        DirectiveError& err) {
    return this->cmp.noOp_directiveHandler(directive, err);
}

Signal FpySequencerTester::tester_waitRel_directiveHandler(const FpySequencer_WaitRelDirective& directive,
                                                           DirectiveError& err) {
    return this->cmp.waitRel_directiveHandler(directive, err);
}

Signal FpySequencerTester::tester_waitAbs_directiveHandler(const FpySequencer_WaitAbsDirective& directive,
                                                           DirectiveError& err) {
    return this->cmp.waitAbs_directiveHandler(directive, err);
}

Signal FpySequencerTester::tester_goto_directiveHandler(const Svc::FpySequencer_GotoDirective& directive,
                                                        DirectiveError& err) {
    return this->cmp.goto_directiveHandler(directive, err);
}

Signal FpySequencerTester::tester_setSerReg_directiveHandler(const FpySequencer_SetSerRegDirective& directive,
                                                               DirectiveError& err) {
    return this->cmp.setSerReg_directiveHandler(directive, err);
}

Signal FpySequencerTester::tester_if_directiveHandler(const FpySequencer_IfDirective& directive, DirectiveError& err) {
    return this->cmp.if_directiveHandler(directive, err);
}

Signal FpySequencerTester::tester_getPrm_directiveHandler(const FpySequencer_GetPrmDirective& directive,
                                                          DirectiveError& err) {
    return this->cmp.getPrm_directiveHandler(directive, err);
}

Signal FpySequencerTester::tester_getTlm_directiveHandler(const FpySequencer_GetTlmDirective& directive,
                                                          DirectiveError& err) {
    return this->cmp.getTlm_directiveHandler(directive, err);
}

Signal FpySequencerTester::tester_exit_directiveHandler(const FpySequencer_ExitDirective& directive,
                                                        DirectiveError& err) {
    return this->cmp.exit_directiveHandler(directive, err);
}

Signal FpySequencerTester::tester_cmd_directiveHandler(const FpySequencer_CmdDirective& directive,
                                                       DirectiveError& err) {
    return this->cmp.cmd_directiveHandler(directive, err);
}

Signal FpySequencerTester::tester_deserSerReg_directiveHandler(const FpySequencer_DeserSerRegDirective& directive,
                                                                 DirectiveError& err) {
    return this->cmp.deserSerReg_directiveHandler(directive, err);
}

Signal FpySequencerTester::tester_binaryRegOp_directiveHandler(const FpySequencer_BinaryRegOpDirective& directive,
                                                             DirectiveError& err) {
    return this->cmp.binaryRegOp_directiveHandler(directive, err);
}

Signal FpySequencerTester::tester_unaryRegOp_directiveHandler(const FpySequencer_UnaryRegOpDirective& directive,
                                                             DirectiveError& err) {
    return this->cmp.unaryRegOp_directiveHandler(directive, err);
}
Signal FpySequencerTester::tester_setReg_directiveHandler(const FpySequencer_SetRegDirective& directive,
                                                          DirectiveError& err) {
    return this->cmp.setReg_directiveHandler(directive, err);
}

Fw::Success FpySequencerTester::tester_deserializeDirective(const Fpy::Statement& stmt,
                                                            Svc::FpySequencer::DirectiveUnion& deserializedDirective) {
    return this->cmp.deserializeDirective(stmt, deserializedDirective);
}

Svc::Signal FpySequencerTester::tester_dispatchStatement() {
    return this->cmp.dispatchStatement();
}

Fw::Success FpySequencerTester::tester_validate() {
    return this->cmp.validate();
}

Svc::Signal FpySequencerTester::tester_checkStatementTimeout() {
    return this->cmp.checkStatementTimeout();
}

Svc::Signal FpySequencerTester::tester_checkShouldWake() {
    return this->cmp.checkShouldWake();
}

Svc::FpySequencer::Telemetry* FpySequencerTester::tester_get_m_tlm_ptr() {
    return &this->cmp.m_tlm;
}

Fw::Success FpySequencerTester::tester_readBytes(Os::File& file, FwSizeType readLen, FpySequencer_FileReadStage readStage, bool updateCrc) {
    return this->cmp.readBytes(file, readLen, readStage, updateCrc);
}

Fw::Success FpySequencerTester::tester_readFooter() {
    return this->cmp.readFooter();
}

Fw::Success FpySequencerTester::tester_readBody() {
    return this->cmp.readBody();
}

Fw::Success FpySequencerTester::tester_readHeader() {
    return this->cmp.readHeader();
}

// Get & Set cmp variables
Fw::String FpySequencerTester::tester_get_m_sequenceFilePath() {
    return this->cmp.m_sequenceFilePath;
}

void FpySequencerTester::tester_set_m_sequenceFilePath(Fw::String str) {
    this->cmp.m_sequenceFilePath = str;
}

U64 FpySequencerTester::tester_get_m_sequencesStarted() {
    return this->cmp.m_sequencesStarted;
}

void FpySequencerTester::tester_set_m_sequencesStarted(U64 val) {
    this->cmp.m_sequencesStarted = val;
}

U64 FpySequencerTester::tester_get_m_statementsDispatched() {
    return this->cmp.m_statementsDispatched;
}

void FpySequencerTester::tester_set_m_statementsDispatched(U64 val) {
    this->cmp.m_statementsDispatched = val;
}

void FpySequencerTester::tester_set_m_computedCRC(U32 crc) {
    this->cmp.m_computedCRC = crc;
}

// Get cmp member pointers
FpySequencer::Runtime* FpySequencerTester::tester_get_m_runtime_ptr() {
    return &(this->cmp.m_runtime);
}

Fpy::Sequence* FpySequencerTester::tester_get_m_sequenceObj_ptr() {
    return &(this->cmp.m_sequenceObj);
}

Fw::ExternalSerializeBuffer* FpySequencerTester::tester_get_m_sequenceBuffer_ptr() {
    return &(this->cmp.m_sequenceBuffer);
}

Svc::FpySequencer::Debug* FpySequencerTester::tester_get_m_debug_ptr() {
    return &(this->cmp.m_debug);
}
I64 FpySequencerTester::tester_binaryRegOp_or(I64 lhs, I64 rhs) {
    return this->cmp.binaryRegOp_or(lhs, rhs);
}
I64 FpySequencerTester::tester_binaryRegOp_and(I64 lhs, I64 rhs) {
    return this->cmp.binaryRegOp_and(lhs, rhs);
}
I64 FpySequencerTester::tester_binaryRegOp_ieq(I64 lhs, I64 rhs) {
    return this->cmp.binaryRegOp_ieq(lhs, rhs);
}
I64 FpySequencerTester::tester_binaryRegOp_ine(I64 lhs, I64 rhs) {
    return this->cmp.binaryRegOp_ine(lhs, rhs);
}
I64 FpySequencerTester::tester_binaryRegOp_ult(I64 lhs, I64 rhs) {
    return this->cmp.binaryRegOp_ult(lhs, rhs);
}
I64 FpySequencerTester::tester_binaryRegOp_ule(I64 lhs, I64 rhs) {
    return this->cmp.binaryRegOp_ule(lhs, rhs);
}
I64 FpySequencerTester::tester_binaryRegOp_ugt(I64 lhs, I64 rhs) {
    return this->cmp.binaryRegOp_ugt(lhs, rhs);
}
I64 FpySequencerTester::tester_binaryRegOp_uge(I64 lhs, I64 rhs) {
    return this->cmp.binaryRegOp_uge(lhs, rhs);
}
I64 FpySequencerTester::tester_binaryRegOp_slt(I64 lhs, I64 rhs) {
    return this->cmp.binaryRegOp_slt(lhs, rhs);
}
I64 FpySequencerTester::tester_binaryRegOp_sle(I64 lhs, I64 rhs) {
    return this->cmp.binaryRegOp_sle(lhs, rhs);
}
I64 FpySequencerTester::tester_binaryRegOp_sgt(I64 lhs, I64 rhs) {
    return this->cmp.binaryRegOp_sgt(lhs, rhs);
}
I64 FpySequencerTester::tester_binaryRegOp_sge(I64 lhs, I64 rhs) {
    return this->cmp.binaryRegOp_sge(lhs, rhs);
}
I64 FpySequencerTester::tester_binaryRegOp_feq(I64 lhs, I64 rhs) {
    return this->cmp.binaryRegOp_feq(lhs, rhs);
}
I64 FpySequencerTester::tester_binaryRegOp_fne(I64 lhs, I64 rhs) {
    return this->cmp.binaryRegOp_fne(lhs, rhs);
}
I64 FpySequencerTester::tester_binaryRegOp_flt(I64 lhs, I64 rhs) {
    return this->cmp.binaryRegOp_flt(lhs, rhs);
}
I64 FpySequencerTester::tester_binaryRegOp_fle(I64 lhs, I64 rhs) {
    return this->cmp.binaryRegOp_fle(lhs, rhs);
}
I64 FpySequencerTester::tester_binaryRegOp_fgt(I64 lhs, I64 rhs) {
    return this->cmp.binaryRegOp_fgt(lhs, rhs);
}
I64 FpySequencerTester::tester_binaryRegOp_fge(I64 lhs, I64 rhs) {
    return this->cmp.binaryRegOp_fge(lhs, rhs);
}
I64 FpySequencerTester::tester_unaryRegOp_not(I64 src) {
    return this->cmp.unaryRegOp_not(src);
}
I64 FpySequencerTester::tester_unaryRegOp_fpext(I64 src) {
    return this->cmp.unaryRegOp_fpext(src);
}
I64 FpySequencerTester::tester_unaryRegOp_fptrunc(I64 src) {
    return this->cmp.unaryRegOp_fptrunc(src);
}
I64 FpySequencerTester::tester_unaryRegOp_fptosi(I64 src) {
    return this->cmp.unaryRegOp_fptosi(src);
}
I64 FpySequencerTester::tester_unaryRegOp_sitofp(I64 src) {
    return this->cmp.unaryRegOp_sitofp(src);
}
I64 FpySequencerTester::tester_unaryRegOp_fptoui(I64 src) {
    return this->cmp.unaryRegOp_fptoui(src);
}
I64 FpySequencerTester::tester_unaryRegOp_uitofp(I64 src) {
    return this->cmp.unaryRegOp_uitofp(src);
}
void FpySequencerTester::tester_doDispatch() {
    this->cmp.doDispatch();
}

Svc::FpySequencer_SequencerStateMachineStateMachineBase::State FpySequencerTester::tester_getState() {
    return this->cmp.m_stateMachine_sequencer.getState();
}

void FpySequencerTester::tester_setState(Svc::FpySequencer_SequencerStateMachineStateMachineBase::State state) {
    FpySequencer_SequencerStateMachineTester::setState(this->cmp.m_stateMachine_sequencer, state);
}

void FpySequencerTester::tester_dispatchDirective(const FpySequencer::DirectiveUnion& directive, const Fpy::DirectiveId& id) {
    this->cmp.dispatchDirective(directive, id);
}

// End UT private/protected access

}  // namespace Svc
