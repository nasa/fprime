module Svc {
module Ccsds {

enum CfdpStatus {
  CFDP_SUCCESS # CFDP operation has been succesfull
  CFDP_ERROR # Generic CFDP error return code
  CFDP_PDU_METADATA_ERROR # Invalid metadata PDU
  CFDP_SHORT_PDU_ERROR # PDU too short
  CFDP_REC_PDU_FSIZE_MISMATCH_ERROR # Receive PDU: EOF file size mismatch
  CFDP_REC_PDU_BAD_EOF_ERROR # Receive PDU: Invalid EOF packet
  CFDP_SEND_PDU_NO_BUF_AVAIL_ERROR # Send PDU: No send buffer available, throttling limit reached
  CFDP_SEND_PDU_ERROR # Send PDU: Send failed
}

# @brief Structure representing base CFDP PDU header
#
# Reflects the common content at the beginning of all CFDP PDUs, of all types.
#
# @sa CF_CFDP_PduHeader_t for encoded form
struct CfdpLogicalPduHeader {
    version: U8 # \brief Version of the protocol#/
    pdu_type: U8 # \brief File Directive (0) or File Data (1)#/
    direction: U8 # \brief Toward Receiver (0) or Toward Sender (1)#/
    txm_mode: U8 # \brief Acknowledged (0) or Unacknowledged (1)#/
    crc_flag: U8 # \brief CRC not present (0) or CRC present (1)#/
    large_flag: U8 # \brief Small/32-bit size (0) or Large/64-bit size (1)#/

    segmentation_control: U8 # \brief Record boundaries not preserved (0) or preserved (1)#/
    eid_length: U8 # \brief Length of encoded entity IDs, in octets (NOT size of logical value)#/
    segment_meta_flag: U8 # \brief Segment Metatdata not present (0) or Present (1)#/
    txn_seq_length: U8 # \brief Length of encoded sequence number, in octets (NOT size of logical value)#/

    header_encoded_length: U16 # \brief Length of the encoded PDU header, in octets (NOT sizeof struct)#/
    data_encoded_length: U16 # \brief Length of the encoded PDU data, in octets#/

    source_eid: CfdpEntityId # \brief Source entity ID (normalized)#/
    destination_eid: CfdpEntityId # \brief Destination entity ID (normalized)#/
    sequence_num: CfdpTransactionSeq # \brief Sequence number (normalized)#/
}

} # Ccsds
} # Svc