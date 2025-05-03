#include "FpySequencerTester.hpp"
namespace Svc {
void FpySequencerTester::addStmt(const Fpy::Statement& stmt) {
    // if fails, cannot add a new stmt (out of space)
    FW_ASSERT(seq.getheader().getstatementCount() < std::numeric_limits<U16>::max());

    seq.getstatements()[seq.getheader().getstatementCount()] = stmt;
    seq.getheader().setstatementCount(seq.getheader().getstatementCount() + 1);
}

void FpySequencerTester::addCmd(FwOpcodeType opcode) {

    Fpy::Statement stmt(Fpy::StatementType::COMMAND, opcode, Fw::StatementArgBuffer(nullptr, 0));
    
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

}