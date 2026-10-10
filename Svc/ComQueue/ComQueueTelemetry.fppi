@ Depth of queues of Fw::ComBuffer type
telemetry comQueueDepth: ComQueueDepth id 0

@ Depth of queues of Fw::Buffer type
telemetry buffQueueDepth: BuffQueueDepth id 1

@ Total number of async ingress drops per Fw::ComBuffer port
telemetry comIngressDropped: ComQueueIngressDrops id 2

@ Total number of async ingress drops per Fw::Buffer port
telemetry buffIngressDropped: BuffQueueIngressDrops id 3

@ Cumulative drops at the managed Fw::ComBuffer FIFOs (including DROP_OLDEST)
telemetry comFifoDropped: ComQueueFifoDrops id 4

@ Cumulative drops at the managed Fw::Buffer FIFOs (including DROP_OLDEST)
telemetry buffFifoDropped: BuffQueueFifoDrops id 5
