module FppTest {
    instance framework: Framework \
        base id 0x3010 \
        queue size 10 \
        stack size 32 * 1024

    instance comp: Comp \
        base id 0x3020 \
        queue size 10 \
        stack size 32 * 1024

    topology SpecialPorts {
        instance framework
        instance comp

        command connections instance framework
        event connections instance framework
        health connections instance framework
        param connections instance framework
        telemetry connections instance framework
        text event connections instance framework
        time connections instance framework

        connections SP {
            comp.Finish -> framework.Finish
        }
    }
}
