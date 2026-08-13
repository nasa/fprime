module TestFlags {
    @ A component with an async serial port. The dispatch code generated for such a port builds
    @ its message buffer as a variable length array, so this component only compiles when the
    @ flags F Prime requires reach the module that owns it.
    active component FlagsComponent {
        async input port serialAsync: serial
    }
}
