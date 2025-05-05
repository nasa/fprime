#####
# CompletionStatus Port:
#
# A port communicating completion status.
#####

module Fw {
    @ Port communicating completion status
    port CompletionStatus(
        completed: Fw.Completed @< Completion status
    )
}