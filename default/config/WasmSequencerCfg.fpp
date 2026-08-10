module Svc {
    module Wasm {
        @ the default value of the SEQ_BASE_DIR parameter. suffixed to
        @ the input sequence file path before resolution occurs following
        @ the rules of Os::File::open. trailing slash optional
        dictionary constant DEFAULT_SEQ_BASE_DIR = ""

        @ Serial port indices for WasmSequencer serialSyncOut port array.
        @ MAX_SERIAL_PORTS must be defined with this exact name for Fpy compiler bounds checking.
        dictionary enum SyncSerialPortIndex: U8 {
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

        @ Serial port indices for WasmSequencer serialAsyncOut port array.
        @ These ports support an asynchronous reply on the [serialAsyncReply] input port.
        @ Sequences will block until a reply (or timeout) is reached
        @ MAX_SERIAL_PORTS must be defined with this exact name for Fpy compiler bounds checking.
        dictionary enum AsyncSerialPortIndex: U8 {
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
