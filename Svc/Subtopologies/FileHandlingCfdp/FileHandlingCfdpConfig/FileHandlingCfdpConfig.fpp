module FileHandlingCfdpConfig {
    # Base ID for the FileHandlingCfdp Subtopology, all components are offsets from this base ID
    constant BASE_ID = 0x06000000

    module QueueSizes {
        constant cfdpManager   = 30
        constant fileManager   = 10
        constant prmDb         = 10
    }

    module StackSizes {
        constant cfdpManager   = 128 * 1024
        constant fileManager   = 64 * 1024
        constant prmDb         = 64 * 1024
    }

    module Priorities {
        constant cfdpManager   = 24
        constant fileManager   = 22
        constant prmDb         = 21
    }
}
