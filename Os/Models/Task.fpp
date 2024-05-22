# ======================================================================
# \title Os/Models/task.fpp
# \brief FPP type definitions for Os/Task.hpp concepts
# ======================================================================

module Os {
@ FPP shadow-enum representing Os::Task::Status
enum TaskStatus {
    OP_OK, @< message sent/received okay
    INVALID_HANDLE, @< Task handle invalid
    INVALID_PARAMS, @< started task with invalid parameters
    INVALID_STACK, @< started with invalid stack size
    UNKNOWN_ERROR, @< unexpected error return value
    INVALID_AFFINITY, @< unable to set the task affinity
    DELAY_ERROR, @< error trying to delay the task
    JOIN_ERROR, @< error trying to join the task
    ERROR_RESOURCES, @< unable to allocate more tasks
    ERROR_PERMISSION, @< permissions error setting-up tasks
    INVALID_STATE, @< Task is in an invalid state for the operation
}
}

