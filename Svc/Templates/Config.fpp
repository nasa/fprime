module Templates {

    @ Generic configuration for an active component instance definition
    struct ActiveComponentConfig {
        queueSize: FwSizeType
        stackSize: FwSizeType
        $priority: FwTaskPriorityType
        cpuAffinity: FwIndexType
    } default {
        queueSize = 10
        stackSize = 64 * 1024
        $priority = 99
        cpuAffinity = Os.TASK_DEFAULT
    }

}
