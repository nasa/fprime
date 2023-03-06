// ======================================================================
// \title  TypeDemo.cpp
// \author mstarch
// \brief  cpp file for TypeDemo component implementation class
// ======================================================================

#include <FpConfig.hpp>
#include <Ref/TypeDemo/TypeDemo.hpp>
#include <limits>

namespace Ref {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

TypeDemo ::TypeDemo(const char* const compName) : TypeDemoComponentBase(compName) {}

void TypeDemo ::init(const NATIVE_INT_TYPE queueDepth, const NATIVE_INT_TYPE instance) {
    TypeDemoComponentBase::init(queueDepth, instance);
}

TypeDemo ::~TypeDemo() {}

// ----------------------------------------------------------------------
// Command handler implementations
// ----------------------------------------------------------------------

void TypeDemo ::CHOICE_cmdHandler(const FwOpcodeType opCode, const U32 cmdSeq, Ref::Choice choice) {
    this->tlmWrite_ChoiceCh(choice);
    this->log_ACTIVITY_HI_ChoiceEv(choice);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void TypeDemo ::CHOICES_cmdHandler(const FwOpcodeType opCode, const U32 cmdSeq, Ref::ManyChoices choices) {
    this->tlmWrite_ChoicesCh(choices);
    this->log_ACTIVITY_HI_ChoicesEv(choices);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}


void TypeDemo ::
    CHOICES_WITH_FRIENDS_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        U8 repeat,
        Ref::ManyChoices choices,
        U8 repeat_max
    )
{
    for (U32 i = 0; (i < std::numeric_limits<U8>::max()) && (i < repeat_max); i++) {
        this->tlmWrite_ChoicesCh(choices);
    }
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
}

void TypeDemo ::
    EXTRA_CHOICES_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        Ref::TooManyChoices choices
    )
{
    this->tlmWrite_ExtraChoicesCh(choices);
    this->log_ACTIVITY_HI_ExtraChoicesEv(choices);
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
}

void TypeDemo ::
    EXTRA_CHOICES_WITH_FRIENDS_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        U8 repeat,
        Ref::TooManyChoices choices,
        U8 repeat_max
    )
{
    for (U32 i = 0; (i < std::numeric_limits<U8>::max()) && (i < repeat_max); i++) {
        this->tlmWrite_ExtraChoicesCh(choices);
    }
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
}


void TypeDemo ::
    CHOICE_PAIR_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        Ref::ChoicePair choices
    )
{
    this->tlmWrite_ChoicePairCh(choices);
    this->log_ACTIVITY_HI_ChoicePairEv(choices);
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
}

void TypeDemo ::
    CHOICE_PAIR_WITH_FRIENDS_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        U8 repeat,
        Ref::ChoicePair choices,
        U8 repeat_max
    )
{
    for (U32 i = 0; (i < std::numeric_limits<U8>::max()) && (i < repeat_max); i++) {
        this->tlmWrite_ChoicePairCh(choices);
    }
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
}

void TypeDemo ::
    GLUTTON_OF_CHOICE_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        Ref::ChoiceSlurry choices
    )
{
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
}

void TypeDemo ::
    GLUTTON_OF_CHOICE_WITH_FRIENDS_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        U8 repeat,
        Ref::ChoiceSlurry choices,
        U8 repeat_max
    )
{
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
}

}  // end namespace Ref
