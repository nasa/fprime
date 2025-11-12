module Svc {
    module Fpy {
        @ the current schema version (must be representable in U8)
        constant SCHEMA_VERSION = 3;

        @ the number of runtime configurable flags. flags modify the sequencer behavior and can be set by the sequence
        # should be equal to (last flag id) + 1
        constant FLAG_COUNT = 1

        enum FlagId : U8 {
            # must start at 0 and increment by 1 each time
            @ if true, the sequence will exit with an error if a command fails
            EXIT_ON_CMD_FAIL = 0
        }

        @ the type which everything referencing a size or offset on the stack is represented in
        # we use a U32 because U16 is too small (would only allow up to 65 kB max stack size)
        type StackSizeType = U32

        enum DirectiveId : U8 {
            INVALID = 0
            WAIT_REL = 1
            WAIT_ABS = 2
            GOTO = 3
            IF = 4
            NO_OP = 5
            PUSH_TLM_VAL = 6
            PUSH_PRM = 7
            CONST_CMD = 8
            # stack op directives
            # all of these are handled at the CPP level by one StackOpDirective to save boilerplate
            # you MUST keep them all in between OR and ITRUNC_64_32 inclusive
            # boolean ops
            OR = 9
            AND = 10
            # integer equalities
            IEQ = 11
            INE = 12
            # unsigned integer inequalities
            ULT = 13
            ULE = 14
            UGT = 15
            UGE = 16
            # signed integer inequalities
            SLT = 17
            SLE = 18
            SGT = 19
            SGE = 20
            # floating point equalities
            FEQ = 21
            FNE = 22
            # floating point inequalities
            FLT = 23
            FLE = 24
            FGT = 25
            FGE = 26
            NOT = 27
            # floating point conversion to signed/unsigned integer,
            # and vice versa
            FPTOSI = 28
            FPTOUI = 29
            SITOFP = 30
            UITOFP = 31
            # integer arithmetic
            ADD = 32
            SUB = 33
            MUL = 34
            UDIV = 35
            SDIV = 36
            UMOD = 37
            SMOD = 38
            # float arithmetic
            FADD = 39
            FSUB = 40
            FMUL = 41
            FDIV = 42
            FPOW = 43
            FLOG = 44
            FMOD = 45
            # floating point bitwidth conversions
            FPEXT = 46
            FPTRUNC = 47
            # integer bitwidth conversions
            # signed integer extend
            SIEXT_8_64 = 48
            SIEXT_16_64 = 49
            SIEXT_32_64 = 50
            # zero (unsigned) integer extend
            ZIEXT_8_64 = 51
            ZIEXT_16_64 = 52
            ZIEXT_32_64 = 53
            # integer truncate
            ITRUNC_64_8 = 54
            ITRUNC_64_16 = 55
            ITRUNC_64_32 = 56
            # end stack op dirs

            EXIT = 57
            ALLOCATE = 58
            STORE_CONST_OFFSET = 59
            LOAD = 60
            PUSH_VAL = 61
            DISCARD = 62
            MEMCMP = 63
            STACK_CMD = 64
            PUSH_TLM_VAL_AND_TIME = 65
            PUSH_TIME = 66
            SET_FLAG = 67
            GET_FLAG = 68
            GET_FIELD = 69
            PEEK = 70
            STORE = 71
        }

        enum DirectiveErrorCode : U8 {
            NO_ERROR = 0
            STMT_OUT_OF_BOUNDS = 1
            TLM_GET_NOT_CONNECTED = 2
            TLM_CHAN_NOT_FOUND = 3
            PRM_GET_NOT_CONNECTED = 4
            PRM_NOT_FOUND = 5
            CMD_SERIALIZE_FAILURE = 6
            EXIT_WITH_ERROR = 7
            STACK_ACCESS_OUT_OF_BOUNDS = 8
            STACK_OVERFLOW = 9
            DOMAIN_ERROR = 10
            FLAG_IDX_OUT_OF_BOUNDS = 11
            ARRAY_OUT_OF_BOUNDS = 12
            ARITHMETIC_OVERFLOW = 13
            ARITHMETIC_UNDERFLOW = 14
        }

        struct Header {
            @ the major version of the FSW
            majorVersion: U8
            @ the minor version of the FSW
            minorVersion: U8
            @ the patch version of the FSW
            patchVersion: U8
            @ the schema version of this file
            schemaVersion: U8

            @ the number of input arguments to this sequence
            @ these will become locals in the sequence
            argumentCount: U8

            @ the number of statements in the sequence
            statementCount: U16

            @ the size of the body in bytes
            bodySize: U32
        } default { majorVersion = 0, minorVersion = 0, patchVersion = 0, schemaVersion = 0, argumentCount = 0, statementCount = 0, bodySize = 0 }

        struct Footer {
            crc: U32
        }

        @ a statement is a directive opcode paired with an argument buffer
        struct Statement {
            opCode: DirectiveId
            argBuf: Fw.StatementArgBuffer
        }

        struct Sequence {
            header: Header
            @ an array of size m_header.argumentCount mapping argument position to local
            @ variable index
            args: [MAX_SEQUENCE_ARG_COUNT] U8
            statements: [MAX_SEQUENCE_STATEMENT_COUNT] Statement
            footer: Footer
        }
    }
}