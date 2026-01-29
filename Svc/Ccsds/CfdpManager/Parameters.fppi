@ CFDP ID to denote the current node when sending PDUs
param LocalEid: CfdpEntityId \
    default 42

@ Maximum number of bytes to put into a file PDU
param OutgoingFileChunkSize: U32 \
    default 480

@ The maximum number of received bytes to calculate a CRC for in a single wakep period
param RxCrcCalcBytesPerWakeup: U32 \
    default 16384

@ Location to store temporary files during uplink transactions
param TmpDir: string size CfdpManagerMaxFileSize \
    default "/tmp"

@ Location to store files that were downlinked from a polling directory, but failed
param FailDir: string size CfdpManagerMaxFileSize \
    default "/fail"

@ Parameter configuration for an array CFDP channels
param ChannelConfig: Cfdp.ChannelArrayParams \
    default [ \
        {
            ack_limit = 4, \
            nack_limit = 4, \
            ack_timer = 3, \
            inactivity_timer = 30, \
            dequeue_enabled = Fw.Enabled.ENABLED, \
            move_dir = "", \
            max_outgoing_pdus_per_cycle = 32 \
        }, \
        {
            ack_limit = 4, \
            nack_limit = 4, \
            ack_timer = 3, \
            inactivity_timer = 30, \
            dequeue_enabled = Fw.Enabled.ENABLED, \
            move_dir = "", \
            max_outgoing_pdus_per_cycle = 32 \
        } \
    ]