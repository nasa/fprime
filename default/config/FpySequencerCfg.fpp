module Svc {
    module Fpy {
        @ The maximum number of arguments a sequence can have
        constant MAX_SEQUENCE_ARG_COUNT = 16
        @ The maximum number of statements a sequence can have
        constant MAX_SEQUENCE_STATEMENT_COUNT = 1024
        @ The maximum number of local variables a sequence can have
        constant MAX_SEQUENCE_LOCAL_VARIABLES = 16
        @ The maximum size a local variable's buffer can be
        # FW_COM_BUFFER_MAX_SIZE - sizeof(FwChanIdType) - sizeof(FwPacketDescriptorType)
        constant MAX_LOCAL_VARIABLE_BUFFER_SIZE = 512 - 4 - 4
    }
}