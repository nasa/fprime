# ======================================================================
# CfdpCfg.fpp
# F Prime CFDP configuration constants
# ======================================================================

@ Number of buffer ports used to send PDUs
@ This must match the CF_NUM_CHANNELS macro defined in CfdpCfg.hpp
constant CfdpManagerNumChannels = 2

@ @brief Entity id size
@
@ @par Description:
@      The maximum size of the entity id as expected for all CFDP packets.
@      CF supports the spec's variable size of EID, where the actual size is
@      selected at runtime, and therefore the size in CFDP PDUs may be smaller
@      than the size specified here.  This type only establishes the maximum
@      size (and therefore maximum value) that an EID may be.
@
@ @note This type is used in several CF commands, and so changing the size
@       of this type will affect the following structs:
@        CF_ConfigTable_t, configuration table - will change size of file
@        CF_ConfigPacket_t, set config params command
@        CF_TxFileCmd_t, transmit file command
@        CF_PlaybackDirCmd_t, equivalent to above
@        CF_Transaction_Payload_t, any command that selects a transaction based on EID
@
@ @par Limits
@         Must be one of U8, U16, U32, U64.
@
@ BPC TODO: Refactor use of CF_EntityId_t to use this type
type CfdpEntityId = U32

@ @brief transaction sequence number size
@
@ @par Description:
@      The max size of the transaction sequence number as expected for all CFDP packets.
@      CF supports the spec's variable size of TSN, where the actual size is
@      selected at runtime, and therefore the size in CFDP PDUs may be smaller
@      than the size specified here.  This type only establishes the maximum
@      size (and therefore maximum value) that a TSN may be.
@
@ @note This type is used in several CF commands, and so changing the size
@       of this type will affect the following structure:
@        CF_Transaction_Payload_t, any command that selects a transaction based on TSN
@
@ @par Limits
@         Must be one of U8, U16, U32, U64.
@
@ BPC TODO: Refactor use of CF_TransactionSeq_t to use this type
type CfdpTransactionSeq = U32