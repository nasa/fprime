module Svc {
    module Fpy {
        @ the current schema version (must be representable in U8)
        constant SCHEMA_VERSION = 1;

        enum DirectiveId : U8 {
            INVALID = 0
            WAIT_REL = 1
            WAIT_ABS = 2
            SET_SER_REG = 3
            GOTO = 4
            IF = 5
            NO_OP = 6
            GET_TLM = 7
            GET_PRM = 8
            CMD = 9
            SET_REG = 10
            DESER_SER_REG_8 = 11
            DESER_SER_REG_4 = 12
            DESER_SER_REG_2 = 13
            DESER_SER_REG_1 = 14
            # binary reg op directives
            # all of these are handled at the CPP level by one BinaryRegOpDirective
            # boolean ops
            OR = 15
            AND = 16
            # integer equalities
            IEQ = 17
            INE = 18
            # unsigned integer inequalities
            ULT = 19
            ULE = 20
            UGT = 21
            UGE = 22
            # signed integer inequalities
            SLT = 23
            SLE = 24
            SGT = 25
            SGE = 26
            # floating point equalities
            FEQ = 27
            FNE = 28
            # floating point inequalities
            FLT = 29
            FLE = 30
            FGT = 31
            FGE = 32
            # end binary reg op directives
            
            # unary reg op dirs
            NOT = 33
            # floating point extension and truncation
            FPEXT = 34
            FPTRUNC = 35
            # floating point conversion to signed/unsigned integer,
            # and vice versa
            FPTOSI = 36
            FPTOUI = 37
            SITOFP = 38
            UITOFP = 39
            # end unary reg op dirs

            EXIT = 40
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