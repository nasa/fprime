module CDHCore {

    instance cmdDisp: Svc.CommandDispatcher base id 0xFF2FF \
        queue size Defaults.QUEUE_SIZE \
        stack size Defaults.STACK_SIZE \
        priority 101

    instance eventLogger: Svc.ActiveLogger base id 0x0B00 \
        queue size Default.queueSize \
        stack size Default.stackSize \
        priority 98

    instance tlmSend: Svc.TlmChan base id 0x0C00 \
        queue size Default.queueSize \
        stack size Default.stackSize \
        priority 97
    
    topology Subtopology {
        instance cmdDisp 
        instance eventLogger
        instance tlmSend

        command connections instance cmdDisp
        event connections instance eventLogger
        telemetry connections instance tlmSend

    } # end topology
} # end CDHCore Subtopology