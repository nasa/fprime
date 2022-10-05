#####
# SuccessCondition:
#
# A port communicating a success or failure condition.
#####

module Fw {
    @ Port communicating success or failure condition
    port SuccessCondition(
        ref condition: Fw.Success @< Condition success/failure
    )
}