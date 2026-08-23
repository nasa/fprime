module Svc {
    module Wasm {
        @ the default value of the SEQ_BASE_DIR parameter. suffixed to
        @ the input sequence file path before resolution occurs following
        @ the rules of Os::File::open. trailing slash optional
        dictionary constant DEFAULT_SEQ_BASE_DIR = ""

        @ Buffer size (bytes) for a WebAssembly module name (LOAD/INVOKE and the
        @ commands/events that reference a loaded module by name)
        dictionary constant MODULE_NAME_STRING_SIZE = 16

        @ Buffer size (bytes) for a WebAssembly global export name
        @ (GLOBAL_SET_*/GLOBAL_GET and their events)
        dictionary constant GLOBAL_NAME_STRING_SIZE = 16

        @ Maximum length (bytes) of a guest-emitted event message
        @ (fprime.event host function)
        dictionary constant GUEST_EVENT_MESSAGE_SIZE = 128

        @ Default value of the INSTRUCTION_FUEL parameter: number of Wasm
        @ instructions to execute per interpreter cycle
        dictionary constant DEFAULT_INSTRUCTION_FUEL = 1000

        dictionary enum SerialPortOutIndex: U8 {
            @ Example serial port 0 - rename to application-specific name (e.g., TIME_SYNC_PORT)
            EXAMPLE_PORT_0 = 0
            @ Example serial port 1 - rename to application-specific name (e.g., SENSOR_DATA_PORT)
            EXAMPLE_PORT_1 = 1
            @ Example serial port 2 - rename to application-specific name
            EXAMPLE_PORT_2 = 2
            @ Example serial port 3 - rename to application-specific name
            EXAMPLE_PORT_3 = 3
            @ Example serial port 4 - rename to application-specific name
            EXAMPLE_PORT_4 = 4

            @ REQUIRED: Maximum number of serial ports. This sentinel value MUST be named
            MAX_SERIAL_PORTS = 5
        }

        dictionary enum SerialPortInIndex: U8 {
            @ Example serial port 0 - rename to application-specific name (e.g., TIME_SYNC_PORT)
            EXAMPLE_PORT_0 = 0
            @ Example serial port 1 - rename to application-specific name (e.g., SENSOR_DATA_PORT)
            EXAMPLE_PORT_1 = 1
            @ Example serial port 2 - rename to application-specific name
            EXAMPLE_PORT_2 = 2
            @ Example serial port 3 - rename to application-specific name
            EXAMPLE_PORT_3 = 3
            @ Example serial port 4 - rename to application-specific name
            EXAMPLE_PORT_4 = 4

            @ REQUIRED: Maximum number of serial ports. This sentinel value MUST be named
            MAX_SERIAL_PORTS = 5
        }
    }
}
