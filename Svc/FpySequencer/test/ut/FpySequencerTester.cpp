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
    seq.get_header().set_schemaVersion(Fpy::SCHEMA_VERSION);
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
    for (U32 ii = 0; ii < seq.get_header().get_argumentCount(); ii++) {
        ASSERT_EQ(buf.serialize(seq.get_args()[ii]), Fw::SerializeStatus::FW_SERIALIZE_OK);
    }
    for (U32 ii = 0; ii < seq.get_header().get_statementCount(); ii++) {
        ASSERT_EQ(buf.serialize(seq.get_statements()[ii]), Fw::SerializeStatus::FW_SERIALIZE_OK);
    }
    seq.get_header().set_bodySize(static_cast<U32>(buf.getBuffLength()));
    buf.resetSer();

    ASSERT_EQ(buf.serialize(seq.get_header()), Fw::SerializeStatus::FW_SERIALIZE_OK);
    for (U32 ii = 0; ii < seq.get_header().get_argumentCount(); ii++) {
        ASSERT_EQ(buf.serialize(seq.get_args()[ii]), Fw::SerializeStatus::FW_SERIALIZE_OK);
    }
    for (U32 ii = 0; ii < seq.get_header().get_statementCount(); ii++) {
        ASSERT_EQ(buf.serialize(seq.get_statements()[ii]), Fw::SerializeStatus::FW_SERIALIZE_OK);
    }

    U32 crc = FpySequencer::CRC_INITIAL_VALUE;
    FpySequencer::updateCrc(crc, buf.getBuffAddr(), buf.getBuffLength());

    seq.get_footer().set_crc(~crc);

    ASSERT_EQ(buf.serialize(seq.get_footer()), Fw::SerializeStatus::FW_SERIALIZE_OK);

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
    FW_ASSERT(seq.get_header().get_statementCount() < std::numeric_limits<U16>::max());

    U16 stateCount = seq.get_header().get_statementCount();
    seq.get_statements()[stateCount] = stmt;
    seq.get_header().set_statementCount(static_cast<U16>(stateCount + 1));
}

void FpySequencerTester::add_WAIT_REL() {
    add_WAIT_REL(FpySequencer_WaitRelDirective());
}

void FpySequencerTester::add_WAIT_REL(FpySequencer_WaitRelDirective dir) {
    Fw::StatementArgBuffer buf;
    addDirective(Fpy::DirectiveId::WAIT_REL, buf);
}

void FpySequencerTester::add_WAIT_ABS() {
    add_WAIT_ABS(FpySequencer_WaitAbsDirective());
}

void FpySequencerTester::add_WAIT_ABS(FpySequencer_WaitAbsDirective dir) {
    Fw::StatementArgBuffer buf;
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

void FpySequencerTester::add_IF(U32 gotoIfFalse) {
    add_IF(FpySequencer_IfDirective(gotoIfFalse));
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

void FpySequencerTester::add_STORE_TLM_VAL(FwChanIdType id, U16 lvarOffset) {
    add_STORE_TLM_VAL(FpySequencer_StoreTlmValDirective(id, lvarOffset));
}

void FpySequencerTester::add_STORE_TLM_VAL(FpySequencer_StoreTlmValDirective dir) {
    Fw::StatementArgBuffer buf;
    FW_ASSERT(buf.serialize(dir) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    addDirective(Fpy::DirectiveId::STORE_TLM_VAL, buf);
}

void FpySequencerTester::add_STORE_PRM(FwPrmIdType id, U16 lvarOffset) {
    add_STORE_PRM(FpySequencer_StorePrmDirective(id, lvarOffset));
}

void FpySequencerTester::add_STORE_PRM(FpySequencer_StorePrmDirective dir) {
    Fw::StatementArgBuffer buf;
    FW_ASSERT(buf.serialize(dir) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    addDirective(Fpy::DirectiveId::STORE_PRM, buf);
}

void FpySequencerTester::add_CONST_CMD(FwOpcodeType opcode) {
    add_CONST_CMD(FpySequencer_ConstCmdDirective(opcode, 0, 0));
}

void FpySequencerTester::add_CONST_CMD(FpySequencer_ConstCmdDirective dir) {
    Fw::StatementArgBuffer buf;
    FW_ASSERT(buf.serialize(dir.get_opCode()) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    FW_ASSERT(buf.serialize(dir.get_argBuf(), dir.get__argBufSize(), Fw::Serialization::OMIT_LENGTH) ==
              Fw::SerializeStatus::FW_SERIALIZE_OK);
    addDirective(Fpy::DirectiveId::CONST_CMD, buf);
}

void FpySequencerTester::add_STACK_OP(Fpy::DirectiveId op) {
    Fw::StatementArgBuffer buf;
    addDirective(op, buf);
}

void FpySequencerTester::add_EXIT() {
    Fw::StatementArgBuffer buf;
    addDirective(Fpy::DirectiveId::EXIT, buf);
}
void FpySequencerTester::add_ALLOCATE(U16 size) {
    add_ALLOCATE(FpySequencer_AllocateDirective(size));
}
void FpySequencerTester::add_ALLOCATE(FpySequencer_AllocateDirective dir) {
    Fw::StatementArgBuffer buf;
    FW_ASSERT(buf.serialize(dir) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    addDirective(Fpy::DirectiveId::ALLOCATE, buf);
}
void FpySequencerTester::add_STORE(U16 lvarOffset, U16 size) {
    add_STORE(FpySequencer_StoreDirective(lvarOffset, size));
}
void FpySequencerTester::add_STORE(FpySequencer_StoreDirective dir) {
    Fw::StatementArgBuffer buf;
    FW_ASSERT(buf.serialize(dir) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    addDirective(Fpy::DirectiveId::STORE, buf);
}
void FpySequencerTester::add_LOAD(U16 lvarOffset, U16 size) {
    add_LOAD(FpySequencer_LoadDirective(lvarOffset, size));
}
void FpySequencerTester::add_LOAD(FpySequencer_LoadDirective dir) {
    Fw::StatementArgBuffer buf;
    FW_ASSERT(buf.serialize(dir) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    addDirective(Fpy::DirectiveId::LOAD, buf);
}
template <typename T>
void FpySequencerTester::add_PUSH_VAL(T val) {
    Fw::StatementArgBuffer buf;
    FW_ASSERT(buf.serializeFrom(val) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    addDirective(Fpy::DirectiveId::PUSH_VAL, buf);
}

template void FpySequencerTester::add_PUSH_VAL(U8);
template void FpySequencerTester::add_PUSH_VAL(U16);
template void FpySequencerTester::add_PUSH_VAL(U32);
template void FpySequencerTester::add_PUSH_VAL(U64);
template void FpySequencerTester::add_PUSH_VAL(I8);
template void FpySequencerTester::add_PUSH_VAL(I16);
template void FpySequencerTester::add_PUSH_VAL(I32);
template void FpySequencerTester::add_PUSH_VAL(I64);
void FpySequencerTester::add_DISCARD(U16 size) {
    add_DISCARD(FpySequencer_DiscardDirective(size));
}
void FpySequencerTester::add_DISCARD(FpySequencer_DiscardDirective dir) {
    Fw::StatementArgBuffer buf;
    FW_ASSERT(buf.serialize(dir) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    addDirective(Fpy::DirectiveId::DISCARD, buf);
}
void FpySequencerTester::add_STACK_CMD(U16 size) {
    add_STACK_CMD(FpySequencer_StackCmdDirective(size));
}
void FpySequencerTester::add_STACK_CMD(FpySequencer_StackCmdDirective dir) {
    Fw::StatementArgBuffer buf;
    FW_ASSERT(buf.serialize(dir) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    addDirective(Fpy::DirectiveId::STACK_CMD, buf);
}
void FpySequencerTester::add_MEMCMP(U16 size) {
    add_MEMCMP(FpySequencer_MemCmpDirective(size));
}
void FpySequencerTester::add_MEMCMP(FpySequencer_MemCmpDirective dir) {
    Fw::StatementArgBuffer buf;
    FW_ASSERT(buf.serialize(dir) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    addDirective(Fpy::DirectiveId::MEMCMP, buf);
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

Signal FpySequencerTester::tester_if_directiveHandler(const FpySequencer_IfDirective& directive, DirectiveError& err) {
    return this->cmp.if_directiveHandler(directive, err);
}

Signal FpySequencerTester::tester_storePrm_directiveHandler(const FpySequencer_StorePrmDirective& directive,
                                                            DirectiveError& err) {
    return this->cmp.storePrm_directiveHandler(directive, err);
}

Signal FpySequencerTester::tester_storeTlmVal_directiveHandler(const FpySequencer_StoreTlmValDirective& directive,
                                                               DirectiveError& err) {
    return this->cmp.storeTlmVal_directiveHandler(directive, err);
}

Signal FpySequencerTester::tester_exit_directiveHandler(const FpySequencer_ExitDirective& directive,
                                                        DirectiveError& err) {
    return this->cmp.exit_directiveHandler(directive, err);
}

Signal FpySequencerTester::tester_constCmd_directiveHandler(const FpySequencer_ConstCmdDirective& directive,
                                                            DirectiveError& err) {
    return this->cmp.constCmd_directiveHandler(directive, err);
}

Signal FpySequencerTester::tester_stackOp_directiveHandler(const FpySequencer_StackOpDirective& directive,
                                                           DirectiveError& err) {
    return this->cmp.stackOp_directiveHandler(directive, err);
}

Signal FpySequencerTester::tester_discard_directiveHandler(const FpySequencer_DiscardDirective& directive,
                                                           DirectiveError& err) {
    return this->cmp.discard_directiveHandler(directive, err);
}

Signal FpySequencerTester::tester_stackCmd_directiveHandler(const FpySequencer_StackCmdDirective& directive,
                                                            DirectiveError& err) {
    return this->cmp.stackCmd_directiveHandler(directive, err);
}

Signal FpySequencerTester::tester_memCmp_directiveHandler(const FpySequencer_MemCmpDirective& directive,
                                                          DirectiveError& err) {
    return this->cmp.memCmp_directiveHandler(directive, err);
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

Fw::Success FpySequencerTester::tester_readBytes(Os::File& file,
                                                 FwSizeType readLen,
                                                 FpySequencer_FileReadStage readStage,
                                                 bool updateCrc) {
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
DirectiveError FpySequencerTester::tester_op_or() {
    return this->cmp.op_or();
}
DirectiveError FpySequencerTester::tester_op_and() {
    return this->cmp.op_and();
}
DirectiveError FpySequencerTester::tester_op_ieq() {
    return this->cmp.op_ieq();
}
DirectiveError FpySequencerTester::tester_op_ine() {
    return this->cmp.op_ine();
}
DirectiveError FpySequencerTester::tester_op_ult() {
    return this->cmp.op_ult();
}
DirectiveError FpySequencerTester::tester_op_ule() {
    return this->cmp.op_ule();
}
DirectiveError FpySequencerTester::tester_op_ugt() {
    return this->cmp.op_ugt();
}
DirectiveError FpySequencerTester::tester_op_uge() {
    return this->cmp.op_uge();
}
DirectiveError FpySequencerTester::tester_op_slt() {
    return this->cmp.op_slt();
}
DirectiveError FpySequencerTester::tester_op_sle() {
    return this->cmp.op_sle();
}
DirectiveError FpySequencerTester::tester_op_sgt() {
    return this->cmp.op_sgt();
}
DirectiveError FpySequencerTester::tester_op_sge() {
    return this->cmp.op_sge();
}
DirectiveError FpySequencerTester::tester_op_feq() {
    return this->cmp.op_feq();
}
DirectiveError FpySequencerTester::tester_op_fne() {
    return this->cmp.op_fne();
}
DirectiveError FpySequencerTester::tester_op_flt() {
    return this->cmp.op_flt();
}
DirectiveError FpySequencerTester::tester_op_fle() {
    return this->cmp.op_fle();
}
DirectiveError FpySequencerTester::tester_op_fgt() {
    return this->cmp.op_fgt();
}
DirectiveError FpySequencerTester::tester_op_fge() {
    return this->cmp.op_fge();
}
DirectiveError FpySequencerTester::tester_op_not() {
    return this->cmp.op_not();
}
DirectiveError FpySequencerTester::tester_op_fpext() {
    return this->cmp.op_fpext();
}
DirectiveError FpySequencerTester::tester_op_fptrunc() {
    return this->cmp.op_fptrunc();
}
DirectiveError FpySequencerTester::tester_op_fptoui() {
    return this->cmp.op_fptoui();
}
DirectiveError FpySequencerTester::tester_op_fptosi() {
    return this->cmp.op_fptosi();
}
DirectiveError FpySequencerTester::tester_op_sitofp() {
    return this->cmp.op_sitofp();
}
DirectiveError FpySequencerTester::tester_op_uitofp() {
    return this->cmp.op_uitofp();
}
DirectiveError FpySequencerTester::tester_op_iadd() {
    return this->cmp.op_iadd();
}
DirectiveError FpySequencerTester::tester_op_isub() {
    return this->cmp.op_isub();
}
DirectiveError FpySequencerTester::tester_op_imul() {
    return this->cmp.op_imul();
}
DirectiveError FpySequencerTester::tester_op_udiv() {
    return this->cmp.op_udiv();
}
DirectiveError FpySequencerTester::tester_op_sdiv() {
    return this->cmp.op_sdiv();
}
DirectiveError FpySequencerTester::tester_op_umod() {
    return this->cmp.op_umod();
}
DirectiveError FpySequencerTester::tester_op_smod() {
    return this->cmp.op_smod();
}
DirectiveError FpySequencerTester::tester_op_fadd() {
    return this->cmp.op_fadd();
}
DirectiveError FpySequencerTester::tester_op_fsub() {
    return this->cmp.op_fsub();
}
DirectiveError FpySequencerTester::tester_op_fmul() {
    return this->cmp.op_fmul();
}
DirectiveError FpySequencerTester::tester_op_fdiv() {
    return this->cmp.op_fdiv();
}
DirectiveError FpySequencerTester::tester_op_float_floor_div() {
    return this->cmp.op_float_floor_div();
}
DirectiveError FpySequencerTester::tester_op_fpow() {
    return this->cmp.op_fpow();
}
DirectiveError FpySequencerTester::tester_op_flog() {
    return this->cmp.op_flog();
}
DirectiveError FpySequencerTester::tester_op_fmod() {
    return this->cmp.op_fmod();
}
DirectiveError FpySequencerTester::tester_op_siext_8_64() {
    return this->cmp.op_siext_8_64();
}
DirectiveError FpySequencerTester::tester_op_siext_16_64() {
    return this->cmp.op_siext_16_64();
}
DirectiveError FpySequencerTester::tester_op_siext_32_64() {
    return this->cmp.op_siext_32_64();
}
DirectiveError FpySequencerTester::tester_op_ziext_8_64() {
    return this->cmp.op_ziext_8_64();
}
DirectiveError FpySequencerTester::tester_op_ziext_16_64() {
    return this->cmp.op_ziext_16_64();
}
DirectiveError FpySequencerTester::tester_op_ziext_32_64() {
    return this->cmp.op_ziext_32_64();
}
DirectiveError FpySequencerTester::tester_op_itrunc_64_8() {
    return this->cmp.op_itrunc_64_8();
}
DirectiveError FpySequencerTester::tester_op_itrunc_64_16() {
    return this->cmp.op_itrunc_64_16();
}
DirectiveError FpySequencerTester::tester_op_itrunc_64_32() {
    return this->cmp.op_itrunc_64_32();
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

void FpySequencerTester::tester_dispatchDirective(const FpySequencer::DirectiveUnion& directive,
                                                  const Fpy::DirectiveId& id) {
    this->cmp.dispatchDirective(directive, id);
}

template <typename T>
void FpySequencerTester::tester_push(T val) {
    cmp.push<T>(val);
}
template void FpySequencerTester::tester_push(U8);
template void FpySequencerTester::tester_push(U16);
template void FpySequencerTester::tester_push(U32);
template void FpySequencerTester::tester_push(U64);
template void FpySequencerTester::tester_push(I8);
template void FpySequencerTester::tester_push(I16);
template void FpySequencerTester::tester_push(I32);
template void FpySequencerTester::tester_push(I64);
template void FpySequencerTester::tester_push(F32);
template void FpySequencerTester::tester_push(F64);
template <typename T>
T FpySequencerTester::tester_pop() {
    return cmp.pop<T>();
}
template U8 FpySequencerTester::tester_pop();
template U16 FpySequencerTester::tester_pop();
template U32 FpySequencerTester::tester_pop();
template U64 FpySequencerTester::tester_pop();
template I8 FpySequencerTester::tester_pop();
template I16 FpySequencerTester::tester_pop();
template I32 FpySequencerTester::tester_pop();
template I64 FpySequencerTester::tester_pop();
template F32 FpySequencerTester::tester_pop();
template F64 FpySequencerTester::tester_pop();
// End UT private/protected access

}  // namespace Svc
