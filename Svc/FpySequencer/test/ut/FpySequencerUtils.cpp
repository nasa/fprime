#include "FpySequencerTester.hpp"
#include <new>
namespace Svc {

// dispatches events from the queue until the component reaches the given state
void FpySequencerTester::dispatchUntilState(State state, U32 bound) {
    U64 iters = 0;
    while (component.sequencer_getState() != state && iters < bound) {
        dispatchCurrentMessages(component);
        iters++;
    }
    ASSERT_EQ(component.sequencer_getState(), state);
}

void FpySequencerTester::assertQueueMsg(FwEnumStoreType msg) {

    // TODO I would like to write this function but I can't have access to the
    // componentipcbuf and msgtypeenum
    // ComponentIpcSerializableBuffer _msg;
    // FwQueuePriorityType _priority = 0;

    // Os::Queue::Status _msgStatus = component.m_queue.receive(
    //   _msg,
    //   Os::Queue::BLOCKING,
    //   _priority
    // );
    // FW_ASSERT(
    //   _msgStatus == Os::Queue::OP_OK,
    //   static_cast<FwAssertArgType>(_msgStatus)
    // );

    // // Reset to beginning of buffer
    // _msg.resetDeser();

    // FwEnumStoreType _desMsg = 0;
    // Fw::SerializeStatus _deserStatus = _msg.deserialize(_desMsg);
    // FW_ASSERT(
    //   _deserStatus == Fw::FW_SERIALIZE_OK,
    //   static_cast<FwAssertArgType>(_deserStatus)
    // );

    // MsgTypeEnum _msgType = static_cast<MsgTypeEnum>(_desMsg);
    // ASSERT_EQ(msg, _msgType);
}

void FpySequencerTester::clearSeq() {
    seq = Fpy::Sequence();
}

void FpySequencerTester::resetRuntime() {
    // explicitly call dtor
    component.m_runtime.~Runtime();
    new (&component.m_runtime) FpySequencer::Runtime();
}

void FpySequencerTester::addStmt(const Fpy::Statement& stmt) {
    // if fails, cannot add a new stmt (out of space)
    FW_ASSERT(seq.getheader().getstatementCount() < std::numeric_limits<U16>::max());

    seq.getstatements()[seq.getheader().getstatementCount()] = stmt;
    seq.getheader().setstatementCount(seq.getheader().getstatementCount() + 1);
}

void FpySequencerTester::addCmd(FwOpcodeType opcode) {
    Fpy::Statement stmt(Fpy::StatementType::COMMAND, opcode, Fw::StatementArgBuffer());

    addStmt(stmt);
}

void FpySequencerTester::addDirective(Fpy::DirectiveId id, Fw::StatementArgBuffer& buf) {
    Fpy::Statement stmt(Fpy::StatementType::DIRECTIVE, static_cast<FwOpcodeType>(id.e), buf);

    addStmt(stmt);
}

void FpySequencerTester::add_WAIT_REL(FpySequencer_WaitRelDirective dir) {
    Fw::StatementArgBuffer buf;
    FW_ASSERT(buf.serialize(dir) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    addDirective(Fpy::DirectiveId::WAIT_REL, buf);
}

void FpySequencerTester::add_WAIT_ABS(FpySequencer_WaitAbsDirective dir) {
    Fw::StatementArgBuffer buf;
    FW_ASSERT(buf.serialize(dir) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    addDirective(Fpy::DirectiveId::WAIT_ABS, buf);
}

void FpySequencerTester::add_GOTO(FpySequencer_GotoDirective dir) {
    Fw::StatementArgBuffer buf;
    FW_ASSERT(buf.serialize(dir) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    addDirective(Fpy::DirectiveId::GOTO, buf);
}

void FpySequencerTester::add_SET_LVAR(FpySequencer_SetLocalVarDirective dir) {
    Fw::StatementArgBuffer buf;
    FW_ASSERT(buf.serialize(dir.getindex()) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    FW_ASSERT(buf.serialize(dir.getvalue(), dir.get_valueSize(), true) == Fw::SerializeStatus::FW_SERIALIZE_OK);
    addDirective(Fpy::DirectiveId::SET_LVAR, buf);
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
}  // namespace Svc