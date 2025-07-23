instance comLog: Svc.ComLogger base id BASE_ID \
    queue size ComLoggerTeeConfig.QueueSizes.comLog \
    stack size ComLoggerTeeConfig.StackSizes.comLog \
    priority ComLoggerTeeConfig.Priorities.comLog

instance comSplitter: Svc.ComSplitter base id BASE_ID + 0x0100

topology Subtopology{
    instance comLog
    instance comSplitter

    connections ComSplitter{
        comSplitter.comOut -> comLog.comIn
    }
}