module Svc {
    module Wasm {
        @ the default value of the SEQ_BASE_DIR parameter: a literal prefix
        @ prepended to each requested sequence file path
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

        @ Maximum number of serialOut ports per WasmSequencer instance.
        @ Each index may specify a distinct purpose.
        @ This contract is specified by the topology connection per-sequencer instance
        dictionary constant MAX_SERIAL_OUT_PORTS = 5

        @ Maximum number of serialIn ports per WasmSequencer instance.
        @ Each index may specify a distinct purpose.
        @ This contract is specified by the topology connection per-sequencer instance
        @ Each serialIn port index will get it's own queue.
        @ When initializing each instance, each port index must be given a queue size.
        dictionary constant MAX_SERIAL_IN_PORTS = 5
    }
}
