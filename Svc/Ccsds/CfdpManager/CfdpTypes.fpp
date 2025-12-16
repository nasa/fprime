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

}
}