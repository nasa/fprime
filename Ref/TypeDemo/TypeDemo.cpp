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

void TypeDemo ::init(const NATIVE_INT_TYPE instance) {
    TypeDemoComponentBase::init(instance);
}

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

void TypeDemo ::CHOICES_WITH_FRIENDS_cmdHandler(const FwOpcodeType opCode,
                                                const U32 cmdSeq,
                                                U8 repeat,
                                                Ref::ManyChoices choices,
                                                U8 repeat_max) {
    for (U32 i = 0; (i < repeat) && (i < std::numeric_limits<U8>::max()) && (i < repeat_max); i++) {
        this->tlmWrite_ChoicesCh(choices);
    }
    this->log_ACTIVITY_HI_ChoicesEv(choices);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void TypeDemo ::EXTRA_CHOICES_cmdHandler(const FwOpcodeType opCode, const U32 cmdSeq, Ref::TooManyChoices choices) {
    this->tlmWrite_ExtraChoicesCh(choices);
    this->log_ACTIVITY_HI_ExtraChoicesEv(choices);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void TypeDemo ::EXTRA_CHOICES_WITH_FRIENDS_cmdHandler(const FwOpcodeType opCode,
                                                      const U32 cmdSeq,
                                                      U8 repeat,
                                                      Ref::TooManyChoices choices,
                                                      U8 repeat_max) {
    for (U32 i = 0; (i < repeat) && (i < std::numeric_limits<U8>::max()) && (i < repeat_max); i++) {
        this->tlmWrite_ExtraChoicesCh(choices);
    }
    this->log_ACTIVITY_HI_ExtraChoicesEv(choices);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void TypeDemo ::CHOICE_PAIR_cmdHandler(const FwOpcodeType opCode, const U32 cmdSeq, Ref::ChoicePair choices) {
    this->tlmWrite_ChoicePairCh(choices);
    this->log_ACTIVITY_HI_ChoicePairEv(choices);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void TypeDemo ::CHOICE_PAIR_WITH_FRIENDS_cmdHandler(const FwOpcodeType opCode,
                                                    const U32 cmdSeq,
                                                    U8 repeat,
                                                    Ref::ChoicePair choices,
                                                    U8 repeat_max) {
    for (U32 i = 0; (i < repeat) && (i < std::numeric_limits<U8>::max()) && (i < repeat_max); i++) {
        this->tlmWrite_ChoicePairCh(choices);
    }
    this->log_ACTIVITY_HI_ChoicePairEv(choices);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void TypeDemo ::GLUTTON_OF_CHOICE_cmdHandler(const FwOpcodeType opCode, const U32 cmdSeq, Ref::ChoiceSlurry choices) {
    this->tlmWrite_ChoiceSlurryCh(choices);
    this->log_ACTIVITY_HI_ChoiceSlurryEv(choices);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void TypeDemo ::GLUTTON_OF_CHOICE_WITH_FRIENDS_cmdHandler(const FwOpcodeType opCode,
                                                          const U32 cmdSeq,
                                                          U8 repeat,
                                                          Ref::ChoiceSlurry choices,
                                                          U8 repeat_max) {
    for (U32 i = 0; (i < repeat) && (i < std::numeric_limits<U8>::max()) && (i < repeat_max); i++) {
        this->tlmWrite_ChoiceSlurryCh(choices);
    }
    this->log_ACTIVITY_HI_ChoiceSlurryEv(choices);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void TypeDemo ::DUMP_TYPED_PARAMETERS_cmdHandler(const FwOpcodeType opCode, const U32 cmdSeq) {
    Fw::ParamValid validity;

    Ref::Choice choice = this->paramGet_CHOICE_PRM(validity);
    this->log_ACTIVITY_HI_ChoicePrmEv(choice, validity);

    Ref::ManyChoices choices = this->paramGet_CHOICES_PRM(validity);
    this->log_ACTIVITY_HI_ChoicesPrmEv(choices, validity);

    Ref::TooManyChoices tooManyChoices = this->paramGet_EXTRA_CHOICES_PRM(validity);
    this->log_ACTIVITY_HI_ExtraChoicesPrmEv(tooManyChoices, validity);

    Ref::ChoicePair choicePair = this->paramGet_CHOICE_PAIR_PRM(validity);
    this->log_ACTIVITY_HI_ChoicePairPrmEv(choicePair, validity);

    Ref::ChoiceSlurry choiceSlurry = this->paramGet_GLUTTON_OF_CHOICE_PRM(validity);
    this->log_ACTIVITY_HI_ChoiceSlurryPrmEv(choiceSlurry, validity);

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void TypeDemo ::DUMP_FLOATS_cmdHandler(const FwOpcodeType opCode, const U32 cmdSeq) {
    Ref::FloatSet invalid;
    invalid[0] = std::numeric_limits<float>::infinity();
    invalid[1] = -1 * std::numeric_limits<float>::infinity();
    invalid[2] = (std::numeric_limits<float>::has_quiet_NaN) ? std::numeric_limits<float>::quiet_NaN() : 0.0f;
    this->log_ACTIVITY_HI_FloatEv(invalid[0], invalid[1], invalid[2], invalid);
    this->tlmWrite_Float1Ch(invalid[0]);
    this->tlmWrite_Float2Ch(invalid[1]);
    this->tlmWrite_Float3Ch(invalid[2]);
    this->tlmWrite_FloatSet(invalid);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}
}  // end namespace Ref
