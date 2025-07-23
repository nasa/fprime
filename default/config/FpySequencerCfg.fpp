module Svc {
    module Fpy {
        @ The maximum number of arguments a sequence can have
        constant MAX_SEQUENCE_ARG_COUNT = 16
        @ The maximum number of statements a sequence can have
        constant MAX_SEQUENCE_STATEMENT_COUNT = 1024
        @ The maximum number of serializable registers a sequence can have
        constant NUM_SERIALIZABLE_REGISTERS = 16
        @ The maximum size a serializable register's buffer can be
        # FW_COM_BUFFER_MAX_SIZE - sizeof(FwChanIdType) - sizeof(FwPacketDescriptorType)
        constant MAX_SERIALIZABLE_REGISTER_SIZE = 512 - 4 - 4

        @ The number of registers available to a sequence
        constant NUM_REGISTERS = 128
    }
}