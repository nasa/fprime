// ======================================================================
// \title  TypeDemo.hpp
// \author mstarch
// \brief  hpp file for TypeDemo component implementation class
// ======================================================================

#ifndef TypeDemo_HPP
#define TypeDemo_HPP

#include "Ref/TypeDemo/TypeDemoComponentAc.hpp"

namespace Ref {

class TypeDemo : public TypeDemoComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct object TypeDemo
    //!
    TypeDemo(const char* const compName  //!< The component name
    );

    //! Initialize object TypeDemo
    //!
    void init(const NATIVE_INT_TYPE instance = 0  //!< The instance number
    );

    //! Destroy object TypeDemo
    //!
    ~TypeDemo() = default;

  PRIVATE:
    // ----------------------------------------------------------------------
    // Command handler implementations
    // ----------------------------------------------------------------------

    //! Implementation for CHOICE command handler
    //! Single choice command
    void CHOICE_cmdHandler(const FwOpcodeType opCode,  //!< The opcode
                           const U32 cmdSeq,           //!< The command sequence number
                           Ref::Choice choice);

    //! Implementation for CHOICES command handler
    //! Multiple choice command via Array
    void CHOICES_cmdHandler(const FwOpcodeType opCode,  //!< The opcode
                            const U32 cmdSeq,           //!< The command sequence number
                            Ref::ManyChoices choices);

    //! Implementation for CHOICES_WITH_FRIENDS command handler
    //! Multiple choice command via Array with a preceding and following argument
    void CHOICES_WITH_FRIENDS_cmdHandler(const FwOpcodeType opCode,  //!< The opcode
                                         const U32 cmdSeq,           //!< The command sequence number
                                         U8 repeat,
                                         Ref::ManyChoices choices,
                                         U8 repeat_max);

    //! Implementation for EXTRA_CHOICES command handler
    //! Multiple choice command via Array
    void EXTRA_CHOICES_cmdHandler(const FwOpcodeType opCode,  //!< The opcode
                                  const U32 cmdSeq,           //!< The command sequence number
                                  Ref::TooManyChoices choices);

    //! Implementation for EXTRA_CHOICES_WITH_FRIENDS command handler
    //! Too many choices command via Array with a preceding and following argument
    void EXTRA_CHOICES_WITH_FRIENDS_cmdHandler(const FwOpcodeType opCode,  //!< The opcode
                                               const U32 cmdSeq,           //!< The command sequence number
                                               U8 repeat,
                                               Ref::TooManyChoices choices,
                                               U8 repeat_max);
    //! Implementation for CHOICE_PAIR command handler
    //! Multiple choice command via Structure
    void CHOICE_PAIR_cmdHandler(const FwOpcodeType opCode,  //!< The opcode
                                const U32 cmdSeq,           //!< The command sequence number
                                Ref::ChoicePair choices);

    //! Implementation for CHOICE_PAIR_WITH_FRIENDS command handler
    //! Multiple choices command via Structure with a preceding and following argument
    void CHOICE_PAIR_WITH_FRIENDS_cmdHandler(const FwOpcodeType opCode,  //!< The opcode
                                             const U32 cmdSeq,           //!< The command sequence number
                                             U8 repeat,
                                             Ref::ChoicePair choices,
                                             U8 repeat_max);
    //! Implementation for GLUTTON_OF_CHOICE command handler
    //! Multiple choice command via Complex Structure
    void GLUTTON_OF_CHOICE_cmdHandler(const FwOpcodeType opCode,  //!< The opcode
                                      const U32 cmdSeq,           //!< The command sequence number
                                      Ref::ChoiceSlurry choices   //!< A phenomenal amount of choice
    );

    //! Implementation for GLUTTON_OF_CHOICE_WITH_FRIENDS command handler
    //! Multiple choices command via Complex Structure with a preceding and following argument
    void GLUTTON_OF_CHOICE_WITH_FRIENDS_cmdHandler(const FwOpcodeType opCode,  //!< The opcode
                                                   const U32 cmdSeq,           //!< The command sequence number
                                                   U8 repeat,  //!< Number of times to repeat the choices
                                                   Ref::ChoiceSlurry choices,  //!< A phenomenal amount of choice
                                                   U8 repeat_max               //!< Limit to the number of repetitions
    );

    //! Implementation for DUMP_TYPED_PARAMETERS command handler
    //! Dump the typed parameters
    void DUMP_TYPED_PARAMETERS_cmdHandler(const FwOpcodeType opCode,  //!< The opcode
                                          const U32 cmdSeq            //!< The command sequence number
    );

    //! Implementation for DUMP_FLOATS command handler
    //!
    void DUMP_FLOATS_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                const U32 cmdSeq           /*!< The command sequence number*/
    );

    //! Implementation for SEND_SCALARS command handler
    //! Send scalars
    void SEND_SCALARS_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                 const U32 cmdSeq,          /*!< The command sequence number*/
                                 Ref::ScalarStruct scalar_input);
};

}  // end namespace Ref

#endif
