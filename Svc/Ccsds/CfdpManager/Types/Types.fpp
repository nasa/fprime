module Svc {
module Ccsds {
module Cfdp {

    # ------------------------------------------------
    # CFDP Types
    # ------------------------------------------------
    enum Status {
        SUCCESS @< CFDP operation has been succesfull
        ERROR @< Generic CFDP error return code
        PDU_METADATA_ERROR @< Invalid metadata PDU
        SHORT_PDU_ERROR @< PDU too short
        REC_PDU_FSIZE_MISMATCH_ERROR @< Receive PDU: EOF file size mismatch
        REC_PDU_BAD_EOF_ERROR @< Receive PDU: Invalid EOF packet
        SEND_PDU_NO_BUF_AVAIL_ERROR @< Send PDU: No send buffer available, throttling limit reached
        SEND_PDU_ERROR @< Send PDU: Send failed
    }

    enum Flow {
        NOT_FROZEN @< CFDP channel operations are executing nominally
        FROZEN @< CFDP channel operations are frozen
    }

    @ Values for CFDP file transfer class
    @
    @ The CFDP specification prescribes two classes/modes of file
    @ transfer protocol operation - unacknowledged/simple or
    @ acknowledged/reliable.
    @
    @ Defined per section 7.1 of CCSDS 727.0-B-5
    enum Class: U8 {
        CLASS_2 = 0 @< CFDP class 2 - Reliable transfer (Acknowledged)
        CLASS_1 = 1 @< CFDP class 1 - Unreliable transfer (Unacknowledged)
    }

    @ Enum used to determine if a file should be kept or deleted after a CFDP transaction
    enum Keep: U8 {
        DELETE = 0 @< File will be deleted after the CFDP transaction
        KEEP = 1 @< File will be kept after the CFDP transaction
    }

    @ CFDP queue identifiers
    enum QueueId: U8 {
        PEND = 0, @< first one on this list is active
        TXA = 1
        TXW = 2
        RX = 3
        HIST = 4
        HIST_FREE = 5
        FREE = 6
        NUM = 7
    }

    @< Structure for configuration parameters for a single CFDP channel
    struct ChannelParams {
        ack_limit: U8 @< number of times to retry ACK (for ex, send FIN and wait for fin-ack)
        nack_limit: U8 @< number of times to retry NAK before giving up (resets on a single response
        ack_timer: U32 @< Acknowledge timer in seconds
        inactivity_timer: U32 @< Inactivity timer in seconds
        dequeue_enabled: Fw.Enabled @< if enabled, then the channel will make pending transactions active
        move_dir: string size CfdpManagerMaxFileSize @< Move directory if not empty
        max_outgoing_pdus_per_cycle: U32 @< Maximum number of PDUs to send per cycle per channel for throttling
    }

    @< Struture for the configured array of CFDP channels
    array ChannelArrayParams = [CfdpManagerNumChannels] ChannelParams

}
}
}
