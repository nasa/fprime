@ CFDP ID to denote the current node when sending PDUs
param LocalEid: CfdpEntityId \
    default 42

@ Maximum number of bytes to put into a file PDU
@ TODO - Should this exist or should this always be CF_MAX_PDU_SIZE - header?
param OutgoingFileChunkSize: U32 \
    default 480

@ The maximum number of received bytes to calculate a CRC for in a single wakup period
@ TODO - I am not sure if this should exist as I do not believe we are porting the concept of wakeup
param RxCrcCalcBytesPerWakeup: U32 \
    default 16384

@ Location to store temporary files during uplink transactions
param TmpDir: string size CfdpManagerMaxFileSize \
    default "/tmp"

@ Location to store files that were downlinked from a polling directory, but failed
param FailDir: string size CfdpManagerMaxFileSize \
    default "/fail"

@ Parameter configuration for an array CFDP channels
param ChannelConfig: CfdpChannelArrayParams \
    default [ \
        {
            ack_limit = 4, \
            nack_limit = 4, \
            ack_timer = 3, \
            inactivity_timer = 30, \
            dequeue_enabled = Fw.Enabled.ENABLED, \
            move_dir = "" \
        }, \
        {
            ack_limit = 4, \
            nack_limit = 4, \
            ack_timer = 3, \
            inactivity_timer = 30, \
            dequeue_enabled = Fw.Enabled.ENABLED, \
            move_dir = "" \
        } \
    ]