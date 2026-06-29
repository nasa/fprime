module Svc {
    module Fpy {
        @ The maximum number of arguments a sequence can have
        dictionary constant MAX_SEQUENCE_ARG_COUNT = 16
        @ The maximum number of statements a sequence can have
        dictionary constant MAX_SEQUENCE_STATEMENT_COUNT = 2048
        @ the maximum number of bytes in a stack
        dictionary constant MAX_STACK_SIZE = 65535
        @ the maximum number of bytes in a directive
        dictionary constant MAX_DIRECTIVE_SIZE = 2048
        @ the default value of the SEQ_BASE_DIR parameter. suffixed to
        @ the input sequence file path before resolution occurs following
        @ the rules of Os::File::open. trailing slash optional
        dictionary constant DEFAULT_SEQ_BASE_DIR = ""

        @ Serial port indices for FpySequencer serialOut port array.
        @ MAX_SERIAL_PORTS must be defined with this exact name for Fpy compiler bounds checking.
        dictionary enum SerialPortIndex : U8 {
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