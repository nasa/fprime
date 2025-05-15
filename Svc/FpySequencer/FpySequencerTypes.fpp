module Svc {
    module Fpy {
        @ the current schema version (must be representable in U8)
        constant SCHEMA_VERSION = 1;

        enum DirectiveId : U32 {
            INVALID = 0x00000000,
            WAIT_REL = 0x00000001,
            WAIT_ABS = 0x00000002,
            SET_LVAR = 0x00000003,
            GOTO = 0x00000004,
            IF = 0x00000005,
            NO_OP = 0x00000006,
            GET_TLM = 0x00000007,
            GET_PRM = 0x00000008,
        }

        enum StatementType : U8 {
            DIRECTIVE = 0,
            COMMAND = 1
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

        struct Statement {
            @ directive or command
            $type: StatementType
            opCode: FwOpcodeType
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