instance comLog: Svc.ComLogger base id BASE_ID \
    queue size ComLogTSplitConfig.QueueSizes.comLog \
    stack size ComLogTSplitConfig.StackSizes.comLog \
    priority ComLogTSplitConfig.Priorities.comLog

instance comSplitter: Svc.ComSplitter base id BASE_ID + 0x0100 \

topology Subtopology{
    instance comLog
    instance comSplitter

    connections ComSplitter{

        comSplitter.comOut -> comLog.comIn

    }
}