module Ref {
    constant dimension = 2

    @ Enumeration type for use later
    enum Choice {
        ONE
        TWO
        RED
        BLUE
    }

    @ Enumeration array
    array ManyChoices = [dimension] Choice

    @ Array of array
    array TooManyChoices = [dimension] ManyChoices

    @ Structure of enums
    struct ChoicePair {
        @ The first choice to make
        firstChoice: Choice,
        @ The second choice to make
        secondChoice: Choice
    }

    @ Structure of enums (with an multi-dimensional array and structure)
    struct ChoiceSlurry {
        @ A large set of disorganized choices
        tooManyChoices: TooManyChoices,
        @ A singular choice
        separateChoice: Choice
        @ A pair of choices
        choicePair: ChoicePair
    }

    @ Component to demonstrate multiple type configurations
    passive component TypeDemo {
        #####
        # Variations on a single enumeration input
        #####
        @ Single choice command
        sync command CHOICE(
            @ A single choice
            choice: Choice
        )

        @ Single choice channel
        telemetry ChoiceCh: Choice

        @ Single choice event
        event ChoiceEv(choice: Choice) severity activity high format "Choice: {}"

        @ Single enumeration parameter
        param CHOICE_PRM: Choice

        #####
        # Variations on a one-dimensional array of enumeration input
        #####
        @ Multiple choice command via Array
        sync command CHOICES(
            @ A set of choices
            choices: ManyChoices
        )

        @ Multiple choice command via Array with a preceding and following argument
        sync command CHOICES_WITH_FRIENDS(
            @ Number of times to repeat the choices
            repeat: U8,
            @ A set of choices
            choices: ManyChoices,
            @ Limit to the number of repetitions
            repeat_max: U8
        )

        @ Multiple choice channel via Array
        telemetry ChoicesCh: ManyChoices

        @ Multiple choice event via Array
        event ChoicesEv(choices: ManyChoices) severity activity high format "Choices: {}"

        @ Multiple enumeration parameter via Array
        param CHOICES_PRM: ManyChoices

        #####
        # Variations on a multi-dimensional array of enumeration input
        #####
        @ Too many choice command via Array
        sync command EXTRA_CHOICES(
            @ Way to many choices to make
            choices: TooManyChoices
        )

        @ Too many choices command via Array with a preceding and following argument
        sync command EXTRA_CHOICES_WITH_FRIENDS(
            @ Number of times to repeat the choices
            repeat: U8,
            @ Way to many choices to make
            choices: TooManyChoices,
            @ Limit to the number of repetitions
            repeat_max: U8
        )

        @ Too many choice channel via Array
        telemetry ExtraChoicesCh: TooManyChoices

        @ Too many choice event via Array
        event ExtraChoicesEv(choices: TooManyChoices) severity activity high format "Choices: {}"

        @ Too many enumeration parameter via Array
        param EXTRA_CHOICES_PRM: ManyChoices

        #####
        # Variations on a basic structure
        #####
        @ Multiple choice command via Structure
        sync command CHOICE_PAIR(
            @ A pair of choices
            choices: ChoicePair
        )

        @ Multiple choices command via Structure with a preceding and following argument
        sync command CHOICE_PAIR_WITH_FRIENDS(
            @ Number of times to repeat the choices
            repeat: U8,
            @ A pair of choices
            choices: ChoicePair,
            @ Limit to the number of repetitions
            repeat_max: U8
        )

        @ Multiple choice channel via Structure
        telemetry ChoicePairCh: ChoicePair

        @ Multiple choice event via Structure
        event ChoicePairEv(choices: ChoicePair) severity activity high format "Choices: {}"

        @ Multiple enumeration parameter via Structure
        param CHOICE_PAIR_PRM: ChoicePair

        #####
        # Variations on a complex structure
        #####
        @ Multiple choice command via Complex Structure
        sync command GLUTTON_OF_CHOICE(
            @ A phenomenal amount of choice
            choices: ChoiceSlurry
        )

        @ Multiple choices command via Complex Structure with a preceding and following argument
        sync command GLUTTON_OF_CHOICE_WITH_FRIENDS(
            @ Number of times to repeat the choices
            repeat: U8,
            @ A phenomenal amount of choice
            choices: ChoiceSlurry,
            @ Limit to the number of repetitions
            repeat_max: U8
        )

        @ Multiple choice channel via Complex Structure
        telemetry ChoiceSlurryCh: ChoiceSlurry

        @ Multiple choice event via Complex Structure
        event ChoiceSlurryEv(choices: ChoiceSlurry) severity activity high format "Choices: {}"

        @ Multiple enumeration parameter via Complex Structure
        param GLUTTON_OF_CHOICE_PRM: ChoiceSlurry

        #####
        # Parameter control: events and a dump command
        #####

        @ Single choice parameter event
        event ChoicePrmEv(choice: Choice, validity: Fw.ParamValid) severity activity high \
            format "CHOICE_PRM: {} with validity: {}"

        @ Multiple choice parameter event via Array
        event ChoicesPrmEv(choices: ManyChoices, validity: Fw.ParamValid) severity activity high \
            format "CHOICES_PRM: {} with validity: {}"

        @ Too many choice parameter event via Array
        event ExtraChoicesPrmEv(choices: TooManyChoices, validity: Fw.ParamValid) severity activity high \
            format "EXTRA_CHOICES_PRM: {} with validity: {}"

        @ Multiple choice parameter event via Structure
        event ChoicePairPrmEv(choices: ChoicePair, validity: Fw.ParamValid) severity activity high \
            format "CHOICE_PAIR_PRM: {} with validity: {}"

        @ Multiple choice parameter event via Complex Structure
        event ChoiceSlurryPrmEv(choices: ChoiceSlurry, validity: Fw.ParamValid) severity activity high \
            format "GLUTTON_OF_CHOICE_PRM: {} with validity: {}"

        @ Dump the typed parameters
        sync command DUMP_TYPED_PARAMETERS()

        # ----------------------------------------------------------------------
        # Special ports
        # ----------------------------------------------------------------------
        @ Time get port
        time get port timeCaller

        @ Command registration port
        command reg port cmdRegOut

        @ Command received port
        command recv port cmdIn

        @ Command response port
        command resp port cmdResponseOut

        @ Text event port
        text event port logTextOut

        @ Event port
        event port logOut

        @ Telemetry port
        telemetry port tlmOut

        @ Parameter get
        param get port prmGetOut

        @ Parameter set
        param set port prmSetOut
    }
}
