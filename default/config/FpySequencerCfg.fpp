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
        @ the base directory relative to which sequence file paths are resolved.
        @ prefixed to the input sequence file path before resolution occurs following
        @ the rules of Os::File::open. trailing slash optional 
        @ a '/' separator is always inserted between this base dir and the input seq path
        dictionary constant SEQ_BASE_DIR = ""
    }
}