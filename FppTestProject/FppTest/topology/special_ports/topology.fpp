module FppTest {
    instance framework: Framework \
        base id 0x3010 \
        queue size 10 \
        stack size 32 * 1024

    instance comp1: Comp \
        base id 0x3020 \
        queue size 10 \
        stack size 32 * 1024

    instance comp2: Comp \
        base id 0x4020 \
        queue size 10 \
        stack size 32 * 1024

    topology SpecialPorts {
        instance framework
        instance comp1
        instance comp2

        command connections instance framework
        event connections instance framework
        health connections instance framework
        param connections instance framework
        telemetry connections instance framework
        text event connections instance framework
        time connections instance framework

        connections SP {
            framework.syncOut -> comp1.syncIn
            framework.syncOut -> comp2.syncIn
            comp1.syncOut -> framework.syncIn
            comp2.syncOut -> framework.syncIn

            comp1.productGetOut -> framework.productGetIn[0]
            comp1.productRequestOut -> framework.productRequestIn[0]
            framework.productResponseOut[0] -> comp1.productRecvIn
            comp1.productSendOut -> framework.productSendIn[0]

            comp2.productGetOut -> framework.productGetIn[1]
            comp2.productRequestOut -> framework.productRequestIn[1]
            framework.productResponseOut[1] -> comp2.productRecvIn
            comp2.productSendOut -> framework.productSendIn[1]
        }
    }
}
