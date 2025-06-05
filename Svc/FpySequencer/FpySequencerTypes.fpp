module Svc {
    module Fpy {
        @ the current schema version (must be representable in U8)
        constant SCHEMA_VERSION = 1;

        enum DirectiveId : U8 {
            INVALID = 0
            WAIT_REL = 1
            WAIT_ABS = 2
            SET_LVAR = 3
            GOTO = 4
            IF = 5
            NO_OP = 6
            GET_TLM = 7
            GET_PRM = 8
            CMD = 9
            SET_REG = 10
            DESER_LVAR_8 = 11
            DESER_LVAR_4 = 12
            DESER_LVAR_2 = 13
            DESER_LVAR_1 = 14
            OR = 15
            EQ = 16
            NE = 17
            # NO REORDER
            # unsigned inequalities
            ULT = 18
            ULE = 19
            UGT = 20
            UGE = 21
            # signed inequalities
            SLT = 22
            SLE = 23
            SGT = 24
            SGE = 25
            # END NO REORDER
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