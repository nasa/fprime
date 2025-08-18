module Svc {
    module Fpy {
        @ the current schema version (must be representable in U8)
        constant SCHEMA_VERSION = 1;

        enum DirectiveId : U8 {
            INVALID = 0
            WAIT_REL = 1
            WAIT_ABS = 2
            GOTO = 4
            IF = 5
            NO_OP = 6
            STORE_TLM_VAL = 7
            STORE_PRM = 8
            CONST_CMD = 9
            # stack op directives
            # all of these are handled at the CPP level by one StackOpDirective
            # boolean ops
            OR = 10
            AND = 11
            # integer equalities
            IEQ = 12
            INE = 13
            # unsigned integer inequalities
            ULT = 14
            ULE = 15
            UGT = 16
            UGE = 17
            # signed integer inequalities
            SLT = 18
            SLE = 19
            SGT = 20
            SGE = 21
            # floating point equalities
            FEQ = 22
            FNE = 23
            # floating point inequalities
            FLT = 24
            FLE = 25
            FGT = 26
            FGE = 27
            NOT = 28
            # floating point conversion to signed/unsigned integer,
            # and vice versa
            FPTOSI = 29
            FPTOUI = 30
            SITOFP = 31
            UITOFP = 32
            # integer arithmetic
            IADD = 33
            ISUB = 34
            IMUL = 35
            UDIV = 36
            SDIV = 37
            UMOD = 38
            SMOD = 39
            # float arithmetic
            FADD = 40
            FSUB = 41
            FMUL = 42
            FDIV = 43
            FLOAT_FLOOR_DIV = 44
            FPOW = 45
            FLOG = 46
            # floating point bitwidth conversions
            FPEXT = 47
            FPTRUNC = 48
            # integer bitwidth conversions
            # signed integer extend
            SIEXT_8_64 = 49
            SIEXT_16_64 = 50
            SIEXT_32_64 = 51
            # zero (unsigned) integer extend
            ZIEXT_8_64 = 52
            ZIEXT_16_64 = 53
            ZIEXT_32_64 = 54
            # integer truncate
            ITRUNC_64_8 = 55
            ITRUNC_64_16 = 56
            ITRUNC_64_32 = 57
            # end stack op dirs

            EXIT = 58
            ALLOCATE = 59
            STORE = 60
            LOAD = 61
            PUSH_VAL = 62
            DISCARD = 63
            MEMCMP = 64
            STACK_CMD = 65
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