module Svc {
    module Fpy {
        @ The maximum number of arguments a sequence can have
        constant MAX_SEQUENCE_ARG_COUNT = 16
        @ The maximum number of statements a sequence can have
        constant MAX_SEQUENCE_STATEMENT_COUNT = 1024
        @ The maximum bytes in a statement arg buffer (should be no greater than the cmd arg buf max size)
        constant MAX_STATEMENT_ARG_BUF_SIZE = 500
    }
}