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
    ASSERT_EQ(Os::FileSystem::removeFile(name), Os::FileSystemInterface::Status::OP_OK);
}

void FpySequencerTester::resetRuntime() {
    // explicitly call dtor
    cmp.m_runtime.~Runtime();
    new (&cmp.m_runtime) FpySequencer::Runtime();
}

void FpySequencerTester::addStmt(const Fpy::Statement& stmt) {
    // if fails, cannot add a new stmt (out of space)
    FW_ASSERT(seq.getheader().getstatementCount() < std::numeric_limits<U16>::max());

    U16 stateCount = seq.getheader().getstatementCount();
    seq.getstatements()[stateCount] = stmt;
    seq.getheader().setstatementCount(static_cast<U16>(stateCount + 1));
}

void FpySequencerTester::addCmd(FwOpcodeType opcode) {
    Fpy::Statement stmt(Fpy::StatementType::COMMAND, opcode, Fw::StatementArgBuffer());

    addStmt(stmt);
}

void FpySequencerTester::addDirective(Fpy::DirectiveId id, Fw::StatementArgBuffer& buf) {
    Fpy::Statement stmt(Fpy::StatementType::DIRECTIVE, static_cast<FwOpcodeType>(id.e), buf);

    addStmt(stmt);
}
void FpySequencerTester::add_WAIT_REL(Fw::TimeInterval duration) {
    add_WAIT_REL(FpySequencer_WaitRelDirective(duration));
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

void FpySequencerTester::add_SET_LVAR(U8 lvarIdx, Fw::StatementArgBuffer value) {
    add_SET_LVAR(FpySequencer_SetLocalVarDirective(lvarIdx, *value.getBuffAddr(), value.getBuffLength()));
}

void FpySequencerTester::add_SET_LVAR(FpySequencer_SetLocalVarDirective dir) {
    Fw::StatementArgBuffer buf;
    FW_ASSERT(buf.serialize(dir.getindex()) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    FW_ASSERT(buf.serialize(dir.getvalue(), dir.get_valueSize(), true) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    addDirective(Fpy::DirectiveId::SET_LVAR, buf);
}

void FpySequencerTester::add_IF(U8 lvarIdx, U32 gotoIfFalse) {
    add_IF(FpySequencer_IfDirective(lvarIdx, gotoIfFalse));
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

void FpySequencerTester::add_GET_TLM(U8 valueDestLvar, U8 timeDestLvar, FwChanIdType id) {
    add_GET_TLM(FpySequencer_GetTlmDirective(valueDestLvar, timeDestLvar, id));
}

void FpySequencerTester::add_GET_TLM(FpySequencer_GetTlmDirective dir) {
    Fw::StatementArgBuffer buf;
    FW_ASSERT(buf.serialize(dir) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    addDirective(Fpy::DirectiveId::GET_TLM, buf);
}

void FpySequencerTester::add_GET_PRM(U8 lvarIdx, FwPrmIdType id) {
    add_GET_PRM(FpySequencer_GetPrmDirective(lvarIdx, id));
}

void FpySequencerTester::add_GET_PRM(FpySequencer_GetPrmDirective dir) {
    Fw::StatementArgBuffer buf;
    FW_ASSERT(buf.serialize(dir) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    addDirective(Fpy::DirectiveId::GET_PRM, buf);
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
}  // namespace Svc
